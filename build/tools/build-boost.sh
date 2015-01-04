#!/bin/bash

# Copyright (c) 2011-2014 CrystaX .NET.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX .NET.

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir>"

PROGRAM_DESCRIPTION=\
"Rebuild Boost libraries for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$BOOST_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build"

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir"

ABIS="$PREBUILT_ABIS"
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs"

TOOLCHAIN_VERSION=4.9
#register_var_option "--toolchain-version=<ver>" TOOLCHAIN_VERSION "Specify toolchain version"

BOOST_VERSION=1.57.0

register_jobs_option

extract_parameters "$@"

BOOST_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$BOOST_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the Boost source tree. See --help"
    exit 1
fi

if [ ! -d "$BOOST_SRCDIR" ]; then
    echo "ERROR: Not a directory: '$BOOST_SRCDIR'"
    exit 1
fi

BOOST_DSTDIR=$NDK_DIR/$BOOST_SUBDIR/$BOOST_VERSION
mkdir -p $BOOST_DSTDIR|| exit 1
fail_panic "Could not create Boost $BOOST_VERSION destination directory: $BOOST_DSTDIR"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-boost
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

mktool()
{
    local tool
    for tool in "$@"; do
        cat >$tool
        fail_panic "Could not create tool $tool"
        chmod +x $tool
        fail_panic "Could not chmod +x $tool"
    done
}

# $1: ABI
# $2: build directory
# $3: build type: "static" or "shared"
build_boost_for_abi ()
{
    local ABI=$1
    local BUILDDIR="$2"
    local TYPE="$3"

    local V
    if [ "$VERBOSE2" = "yes" ]; then
        V=1
    fi

    dump "Building Boost $BOOST_VERSION $TYPE $ABI libraries"

    local APILEVEL=9
    if [ ${ABI%%64*} != ${ABI} ]; then
        APILEVEL=21
    fi

    local PLATFORM="android-$APILEVEL"

    prepare_target_build $ABI $PLATFORM $NDK_DIR
    fail_panic "Could not setup target build"

    rm -Rf $BUILDDIR
    mkdir -p $BUILDDIR
    fail_panic "Couldn't create temporary build directory $BUILDDIR"

    local TCNAME
    case $ABI in
        armeabi*)
            TCNAME=arm-linux-androideabi
            ;;
        arm64*)
            TCNAME=aarch64-linux-android;
            ;;
        mips)
            TCNAME=mipsel-linux-android
            ;;
        mips64)
            TCNAME=mips64el-linux-android
            ;;
        x86|x86_64)
            TCNAME=$ABI
            ;;
        *)
            echo "ERROR: Unknown ABI: $ABI" 1>&2
            exit 1
    esac

    local TCPREFIX
    case $ABI in
        x86)
            TCPREFIX=i686-linux-android
            ;;
        x86_64)
            TCPREFIX=x86_64-linux-android
            ;;
        *)
            TCPREFIX=$TCNAME
    esac

    local ARCH
    case $ABI in
        armeabi*)
            ARCH=arm;
            ;;
        arm64*)
            ARCH=arm64;
            ;;
        *)
            ARCH=$ABI
    esac

    local FLAGS LFLAGS
    case $ABI in
        armeabi)
            FLAGS="-march=armv5te -mtune=xscale -msoft-float"
            ;;
        armeabi-v7a)
            FLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp"
            LFLAGS="-Wl,--fix-cortex-a8"
            ;;
        armeabi-v7a-hard)
            FLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mhard-float"
            LFLAGS="-Wl,--fix-cortex-a8"
            LFLAGS="$LFLAGS -Wl,--no-warn-mismatch"
            ;;
        arm64-v8a)
            FLAGS=""
            ;;
        x86)
            FLAGS="-m32"
            ;;
        x86_64)
            FLAGS="-m64"
            ;;
        mips)
            FLAGS="-mabi=32 -mips32"
            ;;
        mips64)
            FLAGS="-mabi=64 -mips64r6"
            ;;
    esac

    local TCPATH=$NDK_DIR/toolchains/$TCNAME-$TOOLCHAIN_VERSION/prebuilt/$HOST_TAG

    local SAVED_CWD=$(pwd)
    cd $BOOST_SRCDIR || exit 1

    local BOOSTGENFILES="project-config.jam user-config.jam b2 bjam bootstrap.log"
    BOOSTGENFILES="$BOOSTGENFILES tools/build/src/engine/bin.*"
    BOOSTGENFILES="$BOOSTGENFILES tools/build/src/engine/bootstrap"

    rm -Rf $BOOSTGENFILES || exit 1
    trap "rm -Rf $BOOSTGENFILES" EXIT INT QUIT TERM ABRT

    if [ ! -x ./b2 ]; then
        local TMPHOSTTCDIR=$BUILDDIR/host-bin
        run mkdir $TMPHOSTTCDIR || exit 1

        {
            echo "#!/bin/sh"
            echo ""
            echo "exec $CC $HOST_CFLAGS $HOST_LDFLAGS \"\$@\""
        } | mktool $TMPHOSTTCDIR/cc

        PATH=$TMPHOSTTCDIR:$SAVED_PATH
        export PATH

        run ./bootstrap.sh --with-toolset=cc --without-icu
        local RC=$?
        if [ -e bootstrap.log ]; then
            mv -f bootstrap.log $BUILDDIR/
        fi
        if [ $RC -eq 0 ]; then
            true
        else
            false
        fi
        fail_panic "Could not bootstrap Boost build"

        # Remove unneeded intermediate files
        rm -Rf bjam
        rm -Rf tools/build/src/engine/bin.*
        rm -Rf tools/build/src/engine/bootstrap
    fi

    {
        echo "import option ;"
        echo "import feature ;"
        echo "using gcc : $ARCH : g++ ;"
        echo "project : default-build <toolset>gcc ;"
        echo "libraries = ;"
        echo "option.set keep-going : false ;"
    } | cat >project-config.jam
    fail_panic "Could not create project-config.jam"

    {
        echo "using mpi ;"
    } | cat >user-config.jam
    fail_panic "Could not create user-config.jam"

    local TMPTARGETTCDIR=$BUILDDIR/target-bin
    run mkdir $TMPTARGETTCDIR || exit 1

    local SYSROOT=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH
    local LIBCRYSTAX=$NDK_DIR/$CRYSTAX_SUBDIR

    FLAGS="$FLAGS --sysroot=$SYSROOT"

    local TOOL

    for TOOL in gcc g++ c++ cpp; do
        mktool $TMPTARGETTCDIR/$TOOL <<EOF
#!/bin/sh

if echo "\$@" | tr ' ' '\\n' | grep -q -x -e -shared; then
    LINKER=yes
elif echo "\$@" | tr ' ' '\\n' | grep -q -e "^-Wl,"; then
    LINKER=yes
elif echo "\$@" | tr ' ' '\\n' | grep -q -e "^-l"; then
    LINKER=yes
else
    LINKER=no
fi

# Remove any -m32/-m64 from input parameters
PARAMS=\`echo "\$@" | tr ' ' '\\n' | grep -v -x -e -m32 | grep -v -x -e -m64 | tr '\\n' ' '\`
if [ "x\$LINKER" = "xyes" ]; then
    # Fix SONAME for shared boost libraries
    PARAMS=\`echo \$PARAMS | tr ' ' '\\n' | sed 's!^\\(-Wl,libboost_[^\\.]*\\.so\\)\\..*\$!\\1!' | tr '\\n' ' '\`
fi

FLAGS="$FLAGS"
if [ "x\$LINKER" = "xyes" ]; then
    FLAGS="\$FLAGS $LFLAGS"
    FLAGS="\$FLAGS -L$LIBCRYSTAX/libs/$ABI"
else
    FLAGS="\$FLAGS -I$LIBCRYSTAX/include"
fi

exec $TCPATH/bin/$TCPREFIX-$TOOL \$FLAGS \$PARAMS
EOF
        fail_panic "Could not create target tool $TOOL"
    done

    for TOOL in as ar ranlib strip gcc-ar gcc-ranlib; do
        {
            echo "#!/bin/sh"
            echo "exec $TCPATH/bin/$TCPREFIX-$TOOL \"\$@\""
        } | mktool $TMPTARGETTCDIR/$TOOL
        fail_panic "Could not create target tool $TOOL"
    done

    PATH=$TMPTARGETTCDIR:$SAVED_PATH
    export PATH

    local BJAMABI
    case $ARCH in
        arm)
            BJAMARCH=arm
            BJAMABI=aapcs
            ;;
        x86|x86_64)
            BJAMARCH=x86
            BJAMABI=sysv
            ;;
        mips)
            BJAMARCH=mips1
            BJAMABI=o32
            ;;
    esac

    local BJAMADDRMODEL
    if [ ${ARCH%%64} != ${ARCH} ]; then
        BJAMADDRMODEL=64
    else
        BJAMADDRMODEL=32
    fi

    local EXTRA_OPTIONS
    case $ARCH in
        arm|x86|x86_64|mips)
            EXTRA_OPTIONS="$EXTRA_OPTIONS \
                architecture=$BJAMARCH \
                abi=$BJAMABI \
                binary-format=elf \
                address-model=$BJAMADDRMODEL \
                "
            ;;
    esac

    local WITHOUT="--without-python"
    case $ARCH in
        arm64|mips64)
            WITHOUT="$WITHOUT \
                --without-context \
                --without-coroutine \
                "
            ;;
    esac

    local PREFIX=$BUILDDIR/install

    local GNULIBCXX=$NDK_DIR/sources/cxx-stl/gnu-libstdc++/$TOOLCHAIN_VERSION

    run ./b2 -d+2 -q -j$NUM_JOBS \
        variant=release \
        link=$TYPE \
        runtime-link=shared \
        threading=multi \
        threadapi=pthread \
        target-os=android \
        toolset=gcc-$ARCH \
        include=$GNULIBCXX/include \
        include=$GNULIBCXX/libs/$ABI/include \
        library-path=$GNULIBCXX/libs/$ABI \
        linkflags="-lgnustl_${TYPE}" \
        $EXTRA_OPTIONS \
        --layout=system \
        --prefix=$PREFIX \
        --build-dir=$BUILDDIR/build \
        $WITHOUT \
        install \
        || exit 1

    rm -Rf $BOOSTGENFILES || exit 1

    if [ "$BOOST_HEADERS_INSTALLED" != "yes" ]; then
        log "Install Boost $BOOST_VERSION headers into $BOOST_DSTDIR"
        run rm -Rf $BOOST_DSTDIR/include || exit 1
        run cp -pR $PREFIX/include $BOOST_DSTDIR/ || exit 1
        BOOST_HEADERS_INSTALLED=yes
        export BOOST_HEADERS_INSTALLED
    fi

    log "Install Boost $BOOST_VERSION $TYPE $ABI libraries into $BOOST_DSTDIR"
    run mkdir -p $BOOST_DSTDIR/libs/$ABI || exit 1

    local LIBSUFFIX
    if [ "$TYPE" = "shared" ]; then
        LIBSUFFIX=so
    else
        LIBSUFFIX=a
    fi
    run rm -f $BOOST_DSTDIR/libs/$ABI/lib*.$LIBSUFFIX || exit 1

    for f in $(find $PREFIX -name "lib*.$LIBSUFFIX" -print); do
        run cp -pRH $f $BOOST_DSTDIR/libs/$ABI/$(basename $f) || exit 1
    done

    cd $SAVED_CWD || exit 1

    log "Boost $BOOST_VERSION $TYPE $ABI binaries built successfully"
}

SAVED_PATH=$PATH

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="boost-$BOOST_VERSION-headers.tar.bz2"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        BOOST_HEADERS_NEED_PACKAGE=no
    else
        BOOST_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="boost-$BOOST_VERSION-libs-$ABI.tar.bz2"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$BOOST_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_boost_for_abi $ABI "$BUILD_DIR/$ABI/shared" "shared"
        build_boost_for_abi $ABI "$BUILD_DIR/$ABI/static" "static"
    fi
done

# Restore PATH
PATH=$SAVED_PATH
export PATH

# Copy license
log "Copying Boost $BOOST_VERSION license"
run cp -f $BOOST_SRCDIR/LICENSE_1_0.txt $BOOST_DSTDIR/ || exit 1

# Generate Android.mk
log "Generating $BOOST_DSTDIR/Android.mk"
{
    echo "# WARNING!!! THIS IS AUTO-GENERATED FILE!!! DO NOT EDIT IT MANUALLY!!!"
    echo ""
    cat $NDK_DIR/$CRYSTAX_SUBDIR/LICENSE | sed 's,^,# ,' | sed 's,^#\s*$,#,'
    echo ""
    echo 'LOCAL_PATH := $(call my-dir)'

    for suffix in a so; do
        if [ "$suffix" = "a" ]; then
            type=static
        else
            type=shared
        fi
        find $BOOST_DSTDIR/libs -name "libboost_*.$suffix" -exec basename '{}' \; | \
            sed "s,^lib\\([^\\.]*\\)\\.${suffix}$,\\1," | sort | uniq | \
        {
            while read lib; do
                echo ''
                echo 'include $(CLEAR_VARS)'
                echo "LOCAL_MODULE := ${lib}_${type}"
                echo "LOCAL_SRCFILES := libs/\$(TARGET_ARCH_ABI)/lib${lib}.${suffix}"
                echo 'LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include'
                echo "include \$(PREBUILT_$(echo $type | tr '[a-z]' '[A-Z]')_LIBRARY)"
            done
        }
    done
} | cat >$BOOST_DSTDIR/Android.mk

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    if [ "$BOOST_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$BOOST_SUBDIR/$BOOST_VERSION/include"
        PACKAGE_NAME="boost-$BOOST_VERSION-headers.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package Boost $BOOST_VERSION headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES="$BOOST_SUBDIR/$BOOST_VERSION/libs/$ABI"
        PACKAGE_NAME="boost-$BOOST_VERSION-libs-$ABI.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI Boost $BOOST_VERSION binaries!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    done
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    log "Cleaning up..."
    rm -rf $BUILD_DIR
else
    log "Don't forget to cleanup: $BUILD_DIR"
fi

log "Done!"
