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
"Rebuild ICU libraries for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$ICU_SUBDIR, but you can override this with the --out-dir=<path>
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

ICU_VERSION=54.1

register_jobs_option

extract_parameters "$@"

ICU_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$ICU_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the ICU source tree. See --help"
    exit 1
fi

if [ ! -d "$ICU_SRCDIR" ]; then
    echo "ERROR: Not a directory: '$ICU_SRCDIR'"
    exit 1
fi

ICU_DSTDIR=$NDK_DIR/$ICU_SUBDIR/$ICU_VERSION
mkdir -p $ICU_DSTDIR
fail_panic "Could not create ICU $ICU_VERSION destination directory: $ICU_DSTDIR"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-icu
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

prepare_target_build
fail_panic "Could not setup target build"

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

# $1: build directory
build_icu_for_host()
{
    local BUILDDIR="$1"

    dump "Building ICU $ICU_VERSION host ($HOST_OS) binaries"

    rm -Rf $BUILDDIR
    mkdir -p $BUILDDIR
    fail_panic "Couldn't create temporary build directory $BUILDDIR"

    cd $BUILDDIR
    fail_panic "Couldn't CD to temporary build directory $BUILDDIR"

    local TMPHOSTTCDIR=$BUILDDIR/host-bin
    run mkdir $TMPHOSTTCDIR
    fail_panic "Couldn't create temporary directory for host toolchain wrappers"

    {
        echo "#!/bin/sh"
        echo ""
        echo "exec $CC $HOST_CFLAGS $HOST_LDFLAGS \"\$@\""
    } | mktool $TMPHOSTTCDIR/gcc

    {
        echo "#!/bin/sh"
        echo ""
        echo "exec $CXX $HOST_CFLAGS $HOST_LDFLAGS \"\$@\""
    } | mktool $TMPHOSTTCDIR/g++

    PATH=$TMPHOSTTCDIR:$SAVED_PATH
    export PATH

    local ICU_HOST_OS
    case $HOST_OS in
        darwin)
            ICU_HOST_OS=MacOSX/GCC
            ;;
        linux)
            ICU_HOST_OS=Linux/gcc
            ;;
        *)
            echo "ERROR: Unsupported host OS: $HOST_OS" 1>&2
            exit 1
    esac

    run $ICU_SRCDIR/source/runConfigureICU $ICU_HOST_OS
    fail_panic "Couldn't configure host build of ICU $ICU_VERSION"

    run make -j$NUM_JOBS
    fail_panic "Couldn't build host ICU $ICU_VERSION"
}

# $1: ABI
# $2: host ICU build directory
# $3: build directory
build_icu_for_abi ()
{
    local ABI=$1
    local ICUHOSTBUILDDIR="$2"
    local BUILDDIR="$3"

    dump "Building ICU $ICU_VERSION $ABI libraries"

    local APILEVEL=9
    if [ ${ABI%%64*} != ${ABI} ]; then
        APILEVEL=21
    fi

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

    local TMPTARGETTCDIR=$BUILDDIR/target-bin
    run mkdir $TMPTARGETTCDIR
    fail_panic "Couldn't create temporary directory for target $ABI toolchain wrappers"

    local SYSROOT=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH
    local LIBCRYSTAX=$NDK_DIR/$CRYSTAX_SUBDIR
    local GNULIBCXX=$NDK_DIR/sources/cxx-stl/gnu-libstdc++/$TOOLCHAIN_VERSION

    FLAGS="$FLAGS --sysroot=$SYSROOT"
    FLAGS="$FLAGS -fPIC"
    FLAGS="$FLAGS -DU_USING_ICU_NAMESPACE=0"
    FLAGS="$FLAGS -DU_CHARSET_IS_UTF8=1"

    local TOOL

    for TOOL in gcc g++ cpp; do
        mktool $TMPTARGETTCDIR/$TOOL <<EOF
#!/bin/sh

if echo "\$@" | tr ' ' '\\n' | grep -q -x -e -c; then
    LINKER=no
else
    LINKER=yes
fi

# Remove any -m32/-m64 from input parameters
PARAMS=\`echo "\$@" | tr ' ' '\\n' | grep -v -x -e -m32 | grep -v -x -e -m64 | tr '\\n' ' '\`
if [ "x\$LINKER" = "xyes" ]; then
    # Fix SONAME for shared libraries
    NPARAMS=""
    SONAME_DETECTED=no
    for p in \$PARAMS; do
        if [ "x\$p" = "x-Wl,-soname" -o "x\$p" = "x-Wl,-h" ]; then
            SONAME_DETECTED=yes
        elif [ "x\$SONAME_DETECTED" = "xyes" ]; then
            p=\`echo \$p | sed 's!^\\(-Wl,lib[^\\.]*\\.so\\)\\..*\$!\\1!'\`
            SONAME_DETECTED=no
        fi
        NPARAMS="\$NPARAMS \$p"
    done
    PARAMS=\$NPARAMS
fi

FLAGS="$FLAGS"
if [ "x\$LINKER" = "xyes" ]; then
    FLAGS="\$FLAGS $LFLAGS"
    FLAGS="\$FLAGS -L$GNULIBCXX/libs/$ABI"
    FLAGS="\$FLAGS -L$LIBCRYSTAX/libs/$ABI"
else
    FLAGS="\$FLAGS -I$GNULIBCXX/include"
    FLAGS="\$FLAGS -I$GNULIBCXX/libs/$ABI/include"
    FLAGS="\$FLAGS -I$LIBCRYSTAX/include"
fi

PARAMS="\$FLAGS \$PARAMS"
if [ "x\$LINKER" = "xyes" ]; then
    PARAMS="\$PARAMS -lgnustl_shared"
fi

exec $TCPATH/bin/$TCPREFIX-$TOOL \$PARAMS
EOF
        fail_panic "Could not create target tool $TOOL"
    done

    for TOOL in as ar ranlib strip; do
        {
            echo "#!/bin/sh"
            echo "exec $TCPATH/bin/$TCPREFIX-$TOOL \"\$@\""
        } | mktool $TMPTARGETTCDIR/$TOOL
        fail_panic "Could not create target tool $TOOL"
    done

    PATH=$TMPTARGETTCDIR:$SAVED_PATH
    export PATH

    local TMPTARGETBUILD=$BUILDDIR/target-build
    run mkdir -p $TMPTARGETBUILD
    fail_panic "Couldn't create temporary directory for $ABI target build"

    cd $TMPTARGETBUILD
    fail_panic "Couldn't CD to temporary directory for $ABI target build"

    CC=gcc
    CXX=g++
    CPP=cpp
    export CC CXX CPP

    local PREFIX=$BUILDDIR/install

    run $ICU_SRCDIR/source/configure        \
        --prefix=$PREFIX                    \
        --host=$TCPREFIX                    \
        --with-cross-build=$ICUHOSTBUILDDIR \
        --enable-shared                     \
        --enable-static                     \

    fail_panic "Couldn't configure $ABI ICU $ICU_VERSION"

    run make -j$NUM_JOBS
    fail_panic "Couldn't build $ABI ICU $ICU_VERSION"

    run make install
    fail_panic "Couldn't install $ABI ICU $ICU_VERSION"

    if [ "$ICU_HEADERS_INSTALLED" != "yes" ]; then
        log "Install ICU $ICU_VERSION headers into $ICU_DSTDIR"
        run rm -Rf $ICU_DSTDIR/include
        run cp -pR $PREFIX/include $ICU_DSTDIR/
        fail_panic "Couldn't install ICU $ICU_VERSION headers"
        ICU_HEADERS_INSTALLED=yes
        export ICU_HEADERS_INSTALLED
    fi

    log "Install ICU $ICU_VERSION $ABI libraries into $ICU_DSTDIR"
    run mkdir -p $ICU_DSTDIR/libs/$ABI
    fail_panic "Couldn't create ICU $ICU_VERSION target $ABI libraries directory"

    local LIBSUFFIX
    for LIBSUFFIX in a so; do
        run rm -f $ICU_DSTDIR/libs/$ABI/lib*.$LIBSUFFIX

        for f in $(find $PREFIX -name "lib*.$LIBSUFFIX" -print); do
            run cp -pRH $f $ICU_DSTDIR/libs/$ABI/$(basename $f)
            fail_panic "Couldn't install ICU $ICU_VERSION target $ABI $(basename $f)"
        done
    done

    log "ICU $ICU_VERSION $ABI binaries built successfully"
}

SAVED_PATH=$PATH

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="icu-$ICU_VERSION-build-files.tar.bz2"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        ICU_BUILD_FILES_NEED_PACKAGE=no
    else
        ICU_BUILD_FILES_NEED_PACKAGE=yes
    fi

    PACKAGE_NAME="icu-$ICU_VERSION-headers.tar.bz2"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        ICU_HEADERS_NEED_PACKAGE=no
    else
        ICU_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="icu-$ICU_VERSION-libs-$ABI.tar.bz2"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$ICU_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        if [ -z "$ICU_HOST_BUILD" ]; then
            build_icu_for_host "$BUILD_DIR/host"
            ICU_HOST_BUILD="$BUILD_DIR/host"
        fi
        build_icu_for_abi $ABI "$ICU_HOST_BUILD" "$BUILD_DIR/$ABI/shared"
    fi
done

# Restore PATH
PATH=$SAVED_PATH
export PATH

# Copy license
log "Copying ICU $ICU_VERSION license"
run cp -f $ICU_SRCDIR/license.html $ICU_DSTDIR/
fail_panic "Couldn't copy ICU $ICU_VERSION license"

# Generate Android.mk
log "Generating $ICU_DSTDIR/Android.mk"
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
        find $ICU_DSTDIR/libs -name "libicu*.$suffix" -exec basename '{}' \; | \
            sed "s,^lib\\([^\\.]*\\)\\.${suffix}$,\\1," | sort | uniq | \
        {
            while read lib; do
                echo ''
                echo 'include $(CLEAR_VARS)'
                echo "LOCAL_MODULE := ${lib}_${type}"
                echo "LOCAL_SRC_FILES := libs/\$(TARGET_ARCH_ABI)/lib${lib}.${suffix}"
                echo 'LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include'
                echo "include \$(PREBUILT_$(echo $type | tr '[a-z]' '[A-Z]')_LIBRARY)"
            done
        }
    done
} | cat >$ICU_DSTDIR/Android.mk

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    if [ "$ICU_BUILD_FILES_NEED_PACKAGE" = "yes" ]; then
        FILES=""
        for F in Android.mk license.html; do
            FILES="$FILES $ICU_SUBDIR/$ICU_VERSION/$F"
        done
        PACKAGE_NAME="icu-$ICU_VERSION-build-files.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package ICU $ICU_VERSION build files!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    if [ "$ICU_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$ICU_SUBDIR/$ICU_VERSION/include"
        PACKAGE_NAME="icu-$ICU_VERSION-headers.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package ICU $ICU_VERSION headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES="$ICU_SUBDIR/$ICU_VERSION/libs/$ABI"
        PACKAGE_NAME="icu-$ICU_VERSION-libs-$ABI.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI ICU $ICU_VERSION binaries!"
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
