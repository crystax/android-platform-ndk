#!/bin/bash

# Copyright (c) 2011-2015 CrystaX.
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
# THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX.

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir>"

PROGRAM_DESCRIPTION=\
"Rebuild the prebuilt libpng binaries for the Android NDK.

This requires a temporary NDK installation containing platforms and
toolchain binaries for all target architectures, as well as the path to
the corresponding libpng source tree.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$LIBPNG_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>."

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build."

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir."

OUT_DIR=
register_var_option "--out-dir=<path>" OUT_DIR "Specify output directory directly."

ABIS=$(spaces_to_commas $PREBUILT_ABIS)
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs."

LIBPNG_VERSION=
register_var_option "--version=<ver>" LIBPNG_VERSION "Specify libpng version to build"

register_jobs_option

extract_parameters "$@"

LIBPNG_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$LIBPNG_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the libpng source tree. See --help" 1>&2
    exit 1
fi

if [ ! -d "$LIBPNG_SRCDIR" ]; then
    echo "ERROR: No such directory: '$LIBPNG_SRCDIR'" 1>&2
    exit 1
fi

if [ -z "$LIBPNG_VERSION" ]; then
    echo "ERROR: Please specify libpng version" 1>&2
    exit 1
fi

GITHASH=$(git -C $LIBPNG_SRCDIR rev-parse --verify v$LIBPNG_VERSION 2>/dev/null)
if [ -z "$GITHASH" ]; then
    echo "ERROR: Can't find tag v$LIBPNG_VERSION in $LIBPNG_SRCDIR" 1>&2
    exit 1
fi

LIBPNG_DSTDIR=$NDK_DIR/$LIBPNG_SUBDIR/$LIBPNG_VERSION
mkdir -p $LIBPNG_DSTDIR
fail_panic "Can't create libpng-$LIBPNG_VERSION destination directory: $LIBPNG_DSTDIR"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-libpng
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

prepare_target_build
fail_panic "Could not setup target build"

# $1: ABI
# $2: build directory
build_libpng_for_abi()
{
    local ABI="$1"
    local BUILDDIR="$2"

    dump "Building libpng-$LIBPNG_VERSION $ABI libraries"

    local APILEVEL
    case $ABI in
        armeabi*|x86|mips)
            APILEVEL=9
            ;;
        arm64*|x86_64|mips64)
            APILEVEL=21
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local ARCH
    case $ABI in
        armeabi*)
            ARCH=arm
            ;;
        arm64*)
            ARCH=arm64
            ;;
        x86|x86_64|mips|mips64)
            ARCH=$ABI
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local TOOLCHAIN
    case $ABI in
        armeabi*)
            TOOLCHAIN=arm-linux-androideabi
            ;;
        x86)
            TOOLCHAIN=x86
            ;;
        mips)
            TOOLCHAIN=mipsel-linux-android
            ;;
        arm64-v8a)
            TOOLCHAIN=aarch64-linux-android
            ;;
        x86_64)
            TOOLCHAIN=x86_64
            ;;
        mips64)
            TOOLCHAIN=mips64el-linux-android
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local SRCDIR="$BUILDDIR/src"
    run git clone -b v$LIBPNG_VERSION $LIBPNG_SRCDIR $SRCDIR
    fail_panic "Can't copy libpng-$LIBPNG_VERSION sources to temporary directory"

    cd $SRCDIR

    run ./autogen.sh
    fail_panic "Can't create autotools scripts"

    local HOST
    case $ABI in
        armeabi*)
            HOST=arm-linux-androideabi
            ;;
        arm64*)
            HOST=aarch64-linux-android
            ;;
        x86)
            HOST=i686-linux-android
            ;;
        x86_64)
            HOST=x86_64-linux-android
            ;;
        mips)
            HOST=mipsel-linux-android
            ;;
        mips64)
            HOST=mips64el-linux-android
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local INSTALLDIR="$BUILDDIR/install"

    local EXTRA_ARGS=""
    case $ABI in
        armeabi-v7a*)
            EXTRA_ARGS="$EXTRA_ARGS --enable-arm-neon=api"
    esac

    case $ABI in
        armeabi)
            CFLAGS="-march=armv5te -mtune=xscale -msoft-float"
            ;;
        armeabi-v7a)
            CFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp"
            ;;
        armeabi-v7a-hard)
            CFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mhard-float"
            ;;
        *)
            CFLAGS=""
    esac

    case $ABI in
        armeabi*)
            CFLAGS="$CFLAGS -mthumb"
    esac

    CFLAGS="$CFLAGS --sysroot=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH"

    LDFLAGS=""
    if [ "$ABI" = "armeabi-v7a-hard" ]; then
        LDFLAGS="$LDFLAGS -Wl,--no-warn-mismatch"
    fi

    LDFLAGS="$LDFLAGS -L$NDK_DIR/sources/crystax/libs/$ABI"

    local TCPREFIX=$NDK_DIR/toolchains/${TOOLCHAIN}-4.9/prebuilt/$HOST_TAG

    CC=$BUILDDIR/cc
    {
        echo "#!/bin/bash"
        echo "ARGS="
        echo 'NEXT_ARG_IS_SONAME=no'
        echo "for p in \"\$@\"; do"
        echo '    case $p in'
        echo '        -Wl,-soname)'
        echo '            NEXT_ARG_IS_SONAME=yes'
        echo '            ;;'
        echo '        *)'
        echo '            if [ "$NEXT_ARG_IS_SONAME" = "yes" ]; then'
        echo '                p=$(echo $p | sed "s,libpng.*\.so.*$,libpng.so,")'
        echo '                NEXT_ARG_IS_SONAME=no'
        echo '            fi'
        echo '    esac'
        echo "    ARGS=\"\$ARGS \$p\""
        echo "done"
        echo "exec $TCPREFIX/bin/${HOST}-gcc \$ARGS"
    } >$CC
    fail_panic "Can't create cc wrapper"
    chmod +x $CC
    fail_panic "Can't chmod +x cc wrapper"

    CPP="$CC $CFLAGS -E"
    AR=$TCPREFIX/bin/${HOST}-ar
    RANLIB=$TCPREFIX/bin/${HOST}-ranlib
    export CC CPP AR RANLIB
    export CFLAGS LDFLAGS

    run ./configure --prefix=$INSTALLDIR \
        --host=$HOST \
        --enable-shared \
        --enable-static \
        --enable-werror \
        --enable-unversioned-links \
        --with-pic \
        $EXTRA_ARGS
    fail_panic "Can't configure $ABI libpng-$LIBPNG_VERSION"

    run make -j$NUM_JOBS
    fail_panic "Can't build $ABI libpng-$LIBPNG_VERSION"

    run make install
    fail_panic "Can't install $ABI libpng-$LIBPNG_VERSION"

    if [ "$LIBPNG_HEADERS_INSTALLED" != "yes" ]; then
        log "Install libpng-$LIBPNG_VERSION headers into $LIBPNG_DSTDIR"

        run rm -Rf $LIBPNG_DSTDIR/include
        run rsync -aL $INSTALLDIR/include $LIBPNG_DSTDIR/
        fail_panic "Can't install $ABI libpng-$LIBPNG_VERSION headers"

        LIBPNG_HEADERS_INSTALLED=yes
        export LIBPNG_HEADERS_INSTALLED
    fi

    log "Install libpng-$LIBPNG_VERSION $ABI libraries into $LIBPNG_DSTDIR"
    run mkdir -p $LIBPNG_DSTDIR/libs/$ABI
    fail_panic "Can't create libpng-$LIBPNG_VERSION target $ABI libraries directory"

    local LIBSUFFIX
    for LIBSUFFIX in a so; do
        rm -f $LIBPNG_DSTDIR/libs/$ABI/libpng.$LIBSUFFIX
        run rsync -aL $INSTALLDIR/lib/libpng.$LIBSUFFIX $LIBPNG_DSTDIR/libs/$ABI
        fail_panic "Can't install $ABI libpng-$LIBPNG_VERSION libraries"
    done
}

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="libpng-$LIBPNG_VERSION-headers.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        LIBPNG_HEADERS_NEED_PACKAGE=no
    else
        LIBPNG_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE=yes
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="libpng-$LIBPNG_VERSION-libs-$ABI.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$LIBPNG_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE=no
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_libpng_for_abi "$ABI" "$BUILD_DIR/$ABI"
    fi
done

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ]; then
    if [ "$LIBPNG_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$LIBPNG_SUBDIR/$LIBPNG_VERSION/include"
        PACKAGE_NAME="libpng-$LIBPNG_VERSION-headers.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package libpng-$LIBPNG_VERSION headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES="$LIBPNG_SUBDIR/$LIBPNG_VERSION/libs/$ABI"
        PACKAGE_NAME="libpng-$LIBPNG_VERSION-libs-$ABI.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package $ABI libpng-$LIBPNG_VERSION libraries!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    done
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    log "Cleaning up..."
    rm -Rf $BUILD_DIR
else
    log "Don't forget to cleanup: $BUILD_DIR"
fi

log "Done!"
