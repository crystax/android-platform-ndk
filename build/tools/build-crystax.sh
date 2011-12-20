#!/bin/bash
#
# Copyright (C) 2011 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#  This shell script is used to rebuild the prebuilt crystax binaries from
#  their sources. It requires a working NDK installation.
#

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh
. `dirname $0`/builder-funcs.sh

PROGRAM_PARAMETERS=""

PROGRAM_DESCRIPTION=\
"Rebuild the prebuilt crystax binaries for the Android NDK.

This script is called when packaging a new NDK release. It will simply
rebuild the crystax static and shared libraries from sources.

This requires a temporary NDK installation containing platforms and
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

GCC_VERSIONS=$SUPPORTED_GCC_VERSIONS
register_var_option "--gcc-versions=<list>" GCC_VERSIONS "Specify list of GCC versions to build by"

NO_MAKEFILE=
register_var_option "--no-makefile" NO_MAKEFILE "Do not use makefile to speed-up build"

register_jobs_option

extract_parameters "$@"

ABIS=$(commas_to_spaces $ABIS)
GCC_VERSIONS=$(commas_to_spaces $GCC_VERSIONS)

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
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# Location of the crystax source tree
CRYSTAX_SRCDIR=$ANDROID_NDK_ROOT/$CRYSTAX_SUBDIR

# Compiler flags we want to use
CRYSTAX_CFLAGS="-fpic -ffunction-sections -funwind-tables -fstack-protector"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS"  -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -g -O2 -DANDROID -DNDEBUG -Wno-psabi"
CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/include"
for p in include gdtoa locale stdio android ; do
    CRYSTAX_CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/src/$p"
done
CRYSTAX_CXXFLAGS="-fuse-cxa-atexit -fexceptions -frtti"
CRYSTAX_LDFLAGS="-Wl,--no-undefined -Wl,-z,noexecstack"

# List of sources to compile
CRYSTAX_C_SOURCES=$(cd $CRYSTAX_SRCDIR && find src -name '*.c' -print)
CRYSTAX_CPP_SOURCES=$(cd $CRYSTAX_SRCDIR && find src -name '*.cpp' -print)
CRYSTAX_SOURCES="$CRYSTAX_C_SOURCES $CRYSTAX_CPP_SOURCES"

# If the --no-makefile flag is not used, we're going to put all build
# commands in a temporary Makefile that we will be able to invoke with
# -j$NUM_JOBS to build stuff in parallel.
#
if [ -z "$NO_MAKEFILE" ]; then
    MAKEFILE=$BUILD_DIR/Makefile
else
    MAKEFILE=
fi

build_crystax_libs_for_abi ()
{
    local ARCH BINPREFIX
    local ABI=$1
    local GCC_VERSION=$2
    local BUILDDIR="$3"
    local DSTDIR="$4"
    local SRC OBJ OBJECTS CFLAGS CXXFLAGS

    local CFLAGS CXXFLAGS LDFLAGS

    ARCH=$(convert_abi_to_arch $ABI)
    CFLAGS=$CRYSTAX_CFLAGS" -I$CRYSTAX_SRCDIR/src/include/$ARCH"

    CXXFLAGS=$CRYSTAX_CXXFLAGS
    LDFLAGS=$CRYSTAX_LDFLAGS

    if [ "$ABI" = "armeabi" ]; then
        CFLAGS=$CFLAGS" -march=armv5te -msoft-float"
    elif [ "$ABI" = "armeabi-v7a" ]; then
        CFLAGS=$CFLAGS" -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3"
        LDFLAGS=$LDFLAGS" -Wl,--fix-cortex-a8"
    fi

    mkdir -p "$BUILDDIR"

    # If the output directory is not specified, use default location
    if [ -z "$DSTDIR" ]; then
        DSTDIR=$NDK_DIR/$CRYSTAX_SUBDIR/libs/$ABI/$GCC_VERSION
    fi

    mkdir -p "$DSTDIR"

    builder_begin_android $ABI $GCC_VERSION "$BUILDDIR" "$MAKEFILE"
    builder_set_srcdir "$CRYSTAX_SRCDIR"
    builder_set_dstdir "$DSTDIR"

    builder_cflags "$CFLAGS"
    builder_cxxflags "$CXXFLAGS"
    builder_ldflags "$LDFLAGS"
    builder_sources $CRYSTAX_SOURCES

    log "Building $DSTDIR/libcrystax.a"
    builder_static_library libcrystax

    log "Building $DSTDIR/libcrystax.so"
    builder_shared_library libcrystax
    builder_end
}

for ABI in $ABIS; do
    for GCC_VERSION in $GCC_VERSIONS; do
        build_crystax_libs_for_abi $ABI $GCC_VERSION "$BUILD_DIR/$ABI/$GCC_VERSION"
    done
done

exit 0

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    for ABI in $ABIS; do
        for GCC_VERSION in $GCC_VERSIONS; do
            FILES=""
            for LIB in libcrystax.a libcrystax.so; do
                FILES="$FILES $CRYSTAX_SUBDIR/libs/$ABI/$GCC_VERSION/$LIB"
            done
            PACKAGE="$PACKAGE_DIR/crystax-libs-$ABI-$GCC_VERSION.tar.bz2"
            log "Packaging: $PACKAGE"
            pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
            fail_panic "Could not package $ABI-$GCC_VERSION crystax binaries!"
            dump "Packaging: $PACKAGE"
        done
    done
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    log "Cleaning up..."
    rm -rf $BUILD_DIR
else
    log "Don't forget to cleanup: $BUILD_DIR"
fi

log "Done!"
