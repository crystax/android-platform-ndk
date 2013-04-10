#!/bin/bash
#
# Copyright (C) 2011, 2013 The Android Open Source Project
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
#  This shell script is used to rebuild the prebuilt GAbi++ binaries from
#  their sources. It requires a working NDK installation.
#

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh
. `dirname $0`/builder-funcs.sh

PROGRAM_PARAMETERS=""

PROGRAM_DESCRIPTION=\
"Rebuild the prebuilt GAbi++ binaries for the Android NDK.

This script is called when packaging a new NDK release. It will simply
rebuild the GAbi++ static and shared libraries from sources.

This requires a temporary NDK installation containing platforms and
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

If you want use clang to rebuild the binaries, please
use --llvm-version=<ver> option.

The output will be placed in appropriate sub-directories of
<ndk>/$GABIXX_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>."

NDK_DIR=
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build."

OUT_DIR=/tmp/ndk-$USER
OPTION_OUT_DIR=
register_option "--out-dir=<path>" do_out_dir "Specify temporary build dir." "$OUT_DIR"
do_out_dir() { OPTION_OUT_DIR=$1; }

ABIS="$PREBUILT_ABIS"
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs."

NO_MAKEFILE=
register_var_option "--no-makefile" NO_MAKEFILE "Do not use makefile to speed-up build"

VISIBLE_LIBGABIXX_STATIC=
register_var_option "--visible-libgabixx-static" VISIBLE_LIBGABIXX_STATIC "Do not use hidden visibility for libgabi++_static.a"

LLVM_VERSION=
register_var_option "--llvm-version=<ver>" LLVM_VERSION "Specify LLVM version"

register_jobs_option

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

fix_option OUT_DIR "$OPTION_OUT_DIR" "build directory"
setup_default_log_file $OUT_DIR/build.log
OUT_DIR=$OUT_DIR/target/gabi++

run rm -Rf "$OUT_DIR"
run mkdir -p "$OUT_DIR"
fail_panic "Could not create build directory: $OUT_DIR"

# Location of the GAbi++ source tree
GABIXX_SRCDIR=$ANDROID_NDK_ROOT/$GABIXX_SUBDIR

# Compiler flags we want to use
GABIXX_CFLAGS="-fPIC -O2 -DANDROID -D__ANDROID__ -ffunction-sections"
GABIXX_CFLAGS=$GABIXX_CFLAGS" -I$GABIXX_SRCDIR/include"
GABIXX_CXXFLAGS="-fuse-cxa-atexit -fexceptions -frtti"
GABIXX_LDFLAGS="-ldl"

# List of sources to compile
GABIXX_SOURCES=$(cd $GABIXX_SRCDIR && ls src/*.cc)

# If the --no-makefile flag is not used, we're going to put all build
# commands in a temporary Makefile that we will be able to invoke with
# -j$NUM_JOBS to build stuff in parallel.
#
if [ -z "$NO_MAKEFILE" ]; then
    MAKEFILE=$OUT_DIR/Makefile
else
    MAKEFILE=
fi

# build_gabixx_libs_for_abi
# $1: ABI
# $2: build directory
# $3: build type: "static" or "shared"
# $4: (optional) installation directory
build_gabixx_libs_for_abi ()
{
    local ARCH BINPREFIX
    local ABI=$1
    local BUILDDIR="$2"
    local TYPE="$3"
    local DSTDIR="$4"
    local SRC OBJ OBJECTS CFLAGS CXXFLAGS LDFLAGS

    mkdir -p "$BUILDDIR"

    # If the output directory is not specified, use default location
    if [ -z "$DSTDIR" ]; then
        DSTDIR=$NDK_DIR/$GABIXX_SUBDIR/libs/$ABI
    fi

    mkdir -p "$DSTDIR"

    CRYSTAX_SRCDIR=$ANDROID_NDK_ROOT/$CRYSTAX_SUBDIR
    CRYSTAX_INCDIR=$CRYSTAX_SRCDIR/include
    CRYSTAX_LIBDIR=$CRYSTAX_SRCDIR/libs/$ABI

    CFLAGS=$GABIXX_CFLAGS" -I$CRYSTAX_INCDIR"
    CXXFLAGS=$GABIXX_CXXFLAGS
    LDFLAGS=$GABIXX_LDFLAGS" -L$CRYSTAX_LIBDIR -lcrystax"

    builder_begin_android $ABI "$BUILDDIR" "$LLVM_VERSION" "$MAKEFILE"
    builder_set_srcdir "$GABIXX_SRCDIR"
    builder_set_dstdir "$DSTDIR"

    builder_cflags "$CFLAGS"
    if [ "$TYPE" = "static" -a -z "$VISIBLE_LIBGABIXX_STATIC" ]; then
        builder_cxxflags "$CXXFLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
    else
        builder_cxxflags "$CXXFLAGS"
    fi
    builder_ldflags "$LDFLAGS"
    builder_sources $GABIXX_SOURCES

    if [ "$TYPE" = "static" ]; then
        log "Building $DSTDIR/libgabi++_static.a"
        builder_static_library libgabi++_static
    else
        log "Building $DSTDIR/libgabi++_shared.so"
        builder_shared_library libgabi++_shared
    fi
    builder_end
}

for ABI in $ABIS; do
    # try cache
    ARCHIVE="gabixx-libs-$ABI.tar.bz2"
    if [ -n "$PACKAGE_DIR" ]; then
        try_cached_package "$PACKAGE_DIR" "$ARCHIVE" no_exit
        if [ $? = 0 ]; then
            continue
        fi
    fi
    # build from scratch
    build_gabixx_libs_for_abi $ABI "$OUT_DIR/$ABI/shared" "shared"
    build_gabixx_libs_for_abi $ABI "$OUT_DIR/$ABI/static" "static"
    # If needed, package files into tarballs
    if [ -n "$PACKAGE_DIR" ] ; then
        FILES=""
        for LIB in libgabi++_static.a libgabi++_shared.so; do
            FILES="$FILES $GABIXX_SUBDIR/libs/$ABI/$LIB"
        done
        PACKAGE="$PACKAGE_DIR/$ARCHIVE"
        log "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI GAbi++ binaries!"
        dump "Packaged: $PACKAGE"
        cache_package "$PACKAGE_DIR" "$ARCHIVE"
    fi
done

if [ -z "$OPTION_OUT_DIR" ]; then
    log "Cleaning up..."
    rm -rf $OUT_DIR
    dir=`dirname $OUT_DIR`
    while true; do
        rmdir $dir >/dev/null 2>&1 || break
        dir=`dirname $dir`
    done
else
    log "Don't forget to cleanup: $OUT_DIR"
fi

log "Done!"
