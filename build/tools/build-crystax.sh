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
. `dirname $0`/builder-funcs.sh

PROGRAM_PARAMETERS=""

PROGRAM_DESCRIPTION=\
"Rebuild libcrystax for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$CRYSTAX_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>."

NDK_DIR=
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build."

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir."

OUT_DIR=
register_var_option "--out-dir=<path>" OUT_DIR "Specify output directory directly."

ABIS="$PREBUILT_ABIS"
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs."

NO_MAKEFILE=
register_var_option "--no-makefile" NO_MAKEFILE "Do not use makefile to speed-up build"

GCC_VERSION=
register_var_option "--gcc-version=<ver>" GCC_VERSION "Specify GCC version"

LLVM_VERSION=
register_var_option "--llvm-version=<ver>" LLVM_VERSION "Specify LLVM version"

register_jobs_option

register_try64_option

extract_parameters "$@"

ABIS=$(commas_to_spaces $ABIS)

# Handle NDK_DIR
if [ -z "$NDK_DIR" ] ; then
    NDK_DIR=$ANDROID_NDK_ROOT
    log "Auto-config: --ndk-dir=$NDK_DIR"
else
    if [ ! -d "$NDK_DIR" ] ; then
        echo "ERROR: NDK directory does not exists: $NDK_DIR"
        exit 1
    fi
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-crystax
else
    BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# Location of the crystax source tree
STDCXX_SRCDIR=$NDK_DIR/sources/cxx-stl/system
CRYSTAX_SRCDIR=$NDK_DIR/$CRYSTAX_SUBDIR

# Compiler flags we want to use
CRYSTAX_CFLAGS="-fPIC -g -O2 -DANDROID -D__ANDROID__ -DNDEBUG"
CRYSTAX_CFLAGS="$CRYSTAX_CFLAGS -Drestrict=__restrict__ -ffunction-sections -fdata-sections"
#CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack"
#CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$STDCXX_SRCDIR/include"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/include"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/../android/support/src/locale"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/../android/support/src/musl-locale"
for p in $(ls -1d $CRYSTAX_SRCDIR/src/*) ; do
    CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$p"
done
CRYSTAX_ARM_CFLAGS="-marm -mno-unaligned-access"
CRYSTAX_CXXFLAGS="-std=gnu++11 -fuse-cxa-atexit -fno-exceptions -fno-rtti"
CRYSTAX_LDFLAGS="-Wl,--no-undefined -Wl,-z,noexecstack"
CRYSTAX_LDFLAGS=$CRYSTAX_LDFLAGS" -lstdc++ -ldl"

# If the --no-makefile flag is not used, we're going to put all build
# commands in a temporary Makefile that we will be able to invoke with
# -j$NUM_JOBS to build stuff in parallel.
#
if [ -z "$NO_MAKEFILE" ]; then
    MAKEFILE=$BUILD_DIR/Makefile
else
    MAKEFILE=
fi

# $1: ABI
# $2: build directory
# $3: build type: "static" or "shared"
# $4: (optional) installation directory
build_crystax_libs_for_abi ()
{
    local ARCH BINPREFIX
    local ABI=$1
    local BUILDDIR="$2"
    local TYPE="$3"
    local DSTDIR="$4"
    local GCCVER

    mkdir -p "$BUILDDIR"

    # If the output directory is not specified, use default location
    if [ -z "$DSTDIR" ]; then
        DSTDIR=$NDK_DIR/$CRYSTAX_SUBDIR/libs/$ABI
    fi

    mkdir -p "$DSTDIR"

    ARCH=$(convert_abi_to_arch $ABI)

    CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/src/include/$ARCH"
    CXXFLAGS=$CRYSTAX_CXXFLAGS" "$CFLAGS
    LDFLAGS=$CRYSTAX_LDFLAGS

    if [ -n "$GCC_VERSION" ]; then
        GCCVER=$GCC_VERSION
    else
        GCCVER=$(get_default_gcc_version_for_arch $ARCH)
    fi

    builder_begin_android $ABI "$BUILDDIR" "$GCCVER" "$LLVM_VERSION" "$MAKEFILE"
    builder_set_srcdir "$CRYSTAX_SRCDIR"
    builder_set_dstdir "$DSTDIR"

    builder_cflags "$CFLAGS"
    if [ $ABI == "armeabi" -o $ABI == "armeabi-v7a" -o $ABI == "armeabi-v7a-hard" ]; then
        builder_cflags "-D__ARM_EABI__"
        if [ $ABI == "armeabi-v7a-hard" ]; then
            builder_cflags "-mhard-float -D_NDK_MATH_NO_SOFTFP=1"
        fi
    fi

    builder_cxxflags "$CXXFLAGS"

    builder_ldflags "$LDFLAGS"
    if [ $ABI == "armeabi-v7a-hard" ]; then
        builder_cflags "-Wl,--no-warn-mismatch -lm_hard"
    fi

    builder_sources $($CRYSTAX_SRCDIR/bin/list-sources --target=$ABI)

    if [ "$TYPE" = "static" ]; then
        log "Building $DSTDIR/libcrystax.a"
        builder_static_library libcrystax
    else
        log "Building $DSTDIR/libcrystax.so"
        builder_sources src/crystax/android_jni.cpp
        builder_ldflags "-lc"
        if [ $ABI != "armeabi-v7a-hard" ]; then
            builder_ldflags "-lm"
        fi
        builder_shared_library libcrystax
    fi
    builder_end
}

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="crystax-libs-$ABI.tar.bz2"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? = 0 ]; then
            DO_BUILD_PACKAGE="no"
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_crystax_libs_for_abi $ABI "$BUILD_DIR/$ABI/shared" "shared" "$OUT_DIR"
        build_crystax_libs_for_abi $ABI "$BUILD_DIR/$ABI/static" "static" "$OUT_DIR"
    fi
done

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    for ABI in $BUILT_ABIS; do
        FILES=""
        for LIB in libcrystax.a libcrystax.so; do
            FILES="$FILES $CRYSTAX_SUBDIR/libs/$ABI/$LIB"
        done
        PACKAGE_NAME="crystax-libs-$ABI.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        log "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI libcrystax binaries!"
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
