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

BOOSTDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$BOOSTDIR" ]; then
    echo "ERROR: Please provide the path to the Boost source tree. See --help"
    exit 1
fi

if [ ! -d "$BOOSTDIR" ]; then
    echo "ERROR: Not a directory: '$BOOSTDIR'"
    exit 1
fi

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-boost
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

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

    dump "Building $TYPE $ABI Boost libraries"

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

    local ARCHFLAGS
    case $ABI in
        armeabi)
            ARCHFLAGS="-march=armv5te -mtune=xscale -msoft-float"
            ;;
        armeabi-v7a)
            ARCHFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp"
            ;;
        armeabi-v7a-hard)
            ARCHFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mhard-float"
            ;;
        *)
            ARCHFLAGS=""
    esac

    local APILEVEL=9
    if [ ${ABI%%64*} != ${ABI} ]; then
        APILEVEL=21
    fi

    local TCPATH=$NDK_DIR/toolchains/$TCNAME-$TOOLCHAIN_VERSION/prebuilt/$HOST_TAG

    cd $BOOSTDIR || exit 1

    local BOOSTGENFILES="project-config.jam user-config.jam b2 bjam bootstrap.log"
    BOOSTGENFILES="$BOOSTGENFILES tools/build/src/engine/bin.*"
    BOOSTGENFILES="$BOOSTGENFILES tools/build/src/engine/bootstrap"

    rm -Rf $BOOSTGENFILES || exit 1
    trap "rm -Rf $BOOSTGENFILES" EXIT INT QUIT TERM ABRT

    run ./bootstrap.sh --without-icu || exit 1

    cat >project-config.jam <<EOF
import option ;
import feature ;

using gcc : $ARCH : g++ ;

project : default-build <toolset>gcc ;

libraries = ;

# Stop on first error
option.set keep-going : false ;
EOF
    test $? -eq 0 || exit 1

    cat >user-config.jam <<EOF
using mpi ;
EOF
    test $? -eq 0 || exit 1

    local TMPTCDIR=$BUILDDIR/bin
    run mkdir $TMPTCDIR || exit 1

    local TOOL

    # gcc
    TOOL=$TMPTCDIR/gcc
    cat >$TOOL <<EOF
#!/bin/sh

exec $TCPATH/bin/$TCPREFIX-gcc $ARCHFLAGS "\$@"
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    # g++
    TOOL=$TMPTCDIR/g++
    cat >$TOOL <<EOF
#!/bin/sh

PARAMS=\`echo "\$@" | tr ' ' '\\n' | sed 's!^\\(-Wl,libboost_[^\\.]*\\.so\\)\\..*\$!\\1!' | tr '\\n' ' '\`
exec $TCPATH/bin/$TCPREFIX-g++ $ARCHFLAGS \$PARAMS
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    # cpp
    TOOL=$TMPTCDIR/cpp
    cat >$TOOL <<EOF
#!/bin/sh

exec $TCPATH/bin/$TCPREFIX-cpp $ARCHFLAGS "\$@"
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    # as
    TOOL=$TMPTCDIR/as
    cat >$TOOL <<EOF
#!/bin/sh

exec $TCPATH/bin/$TCPREFIX-as "\$@"
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    # ar
    TOOL=$TMPTCDIR/ar
    cat >$TOOL <<EOF
#!/bin/sh

exec $TCPATH/bin/$TCPREFIX-ar "\$@"
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    # ranlib
    TOOL=$TMPTCDIR/ranlib
    cat >$TOOL <<EOF
#!/bin/sh

exec $TCPATH/bin/$TCPREFIX-ranlib "\$@"
EOF
    test $? -eq 0 || exit 1
    chmod +x $TOOL || exit 1

    PATH=$TMPTCDIR:$SAVED_PATH
    export PATH

    local SYSROOT=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH
    local GNULIBCXX=$NDK_DIR/sources/cxx-stl/gnu-libstdc++/$TOOLCHAIN_VERSION

    local EXTRA_FLAGS=""
    case $ARCH in
        arm)
            EXTRA_FLAGS="$EXTRA_FLAGS abi=aapcs binary-format=elf"
            ;;
    esac

    if [ "$TYPE" = "shared" ]; then
        EXTRA_FLAGS="$EXTRA_FLAGS linkflags=-lgnustl_shared"
    fi

    local PREFIX=$BUILDDIR/install

    run ./b2 -d+2 -q -j$NUM_JOBS \
        variant=release \
        link=$TYPE \
        runtime-link=shared \
        threading=multi \
        threadapi=pthread \
        target-os=android \
        toolset=gcc-$ARCH \
        architecture=$ARCH \
        include=$GNULIBCXX/include \
        include=$GNULIBCXX/libs/$ABI/include \
        include=$NDK_DIR/$CRYSTAX_SUBDIR/include \
        library-path=$GNULIBCXX/libs/$ABI \
        library-path=$NDK_DIR/$CRYSTAX_SUBDIR/libs/$ABI \
        cflags="--sysroot=$SYSROOT" \
        cxxflags="--sysroot=$SYSROOT" \
        linkflags="--sysroot=$SYSROOT" \
        $EXTRA_FLAGS \
        --without-python \
        --layout=system \
        --prefix=$PREFIX \
        --build-dir=$BUILDDIR/build \
        install \
        || exit 1

    rm -Rf $BOOSTGENFILES || exit 1

    local INSTALL=$NDK_DIR/$BOOST_SUBDIR/$BOOST_VERSION

    mkdir -p $INSTALL || exit 1

    if [ "$BOOST_HEADERS_INSTALLED" != "yes" ]; then
        run rm -Rf $INSTALL/include || exit 1
        run cp -pR $PREFIX/include $INSTALL/ || exit 1
        BOOST_HEADERS_INSTALLED=yes
        export BOOST_HEADERS_INSTALLED
    fi

    run mkdir -p $INSTALL/libs/$ABI || exit 1

    local LIBSUFFIX
    if [ "$TYPE" = "shared" ]; then
        LIBSUFFIX=so
    else
        LIBSUFFIX=a
    fi
    run rm -f $INSTALL/libs/$ABI/lib*.$LIBSUFFIX || exit 1

    for f in $(find $PREFIX -name "lib*.$LIBSUFFIX" -print); do
        run cp -pRH $f $INSTALL/libs/$ABI/$(basename $f) || exit 1
    done
}

SAVED_PATH=$PATH

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="boost-libs-$ABI.tar.bz2"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? = 0 ]; then
            DO_BUILD_PACKAGE="no"
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_boost_for_abi $ABI "$BUILD_DIR/$ABI/shared" "shared"
        build_boost_for_abi $ABI "$BUILD_DIR/$ABI/static" "static"
    fi
done

PATH=$SAVED_PATH
export PATH

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    for ABI in $BUILT_ABIS; do
        FILES=""
        for MLIB in $($NDK_DIR/$CRYSTAX_SUBDIR/bin/config --multilibs --abi=$ABI); do
            LIBPATH=$($NDK_DIR/$CRYSTAX_SUBDIR/bin/config --libpath --abi=$ABI --multilib=$MLIB)
            for LIB in libboost_*.so libboost_*.a; do
                FILES="$FILES $CRYSTAX_SUBDIR/$LIBPATH/$LIB"
            done
        done
        PACKAGE_NAME="boost-libs-$ABI.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        log "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI Boost binaries!"
        dump "Packaging: $PACKAGE"
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
