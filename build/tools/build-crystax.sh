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
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build"

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir"

ABIS="$PREBUILT_ABIS"
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs"

TOOLCHAIN_VERSION=gcc4.9
register_var_option "--toolchain-version=<ver>" TOOLCHAIN_VERSION "Specify toolchain version"

PATCH_SYSROOT=
register_option "--patch-sysroot" do_patch_sysroot "Patch sysroot with CrystaX libraries after build"
do_patch_sysroot() { PATCH_SYSROOT=yes; }

register_jobs_option

register_try64_option

extract_parameters "$@"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-crystax
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# $1: ABI
# $2: build directory
# $3: build type: "static" or "shared"
build_crystax_libs_for_abi ()
{
    local ABI=$1
    local OBJDIR="$2"
    local TYPE="$3"

    local V
    if [ "$VERBOSE2" = "yes" ]; then
        V=1
    fi

    dump "Building $TYPE $ABI libcrystax"

    rm -Rf $OBJDIR
    mkdir -p $OBJDIR
    fail_panic "Couldn't create temporary build directory $OBJDIR"

    run make -C $NDK_DIR/$CRYSTAX_SUBDIR -j$NUM_JOBS $TYPE V=$V NDK=$NDK_DIR ABI=$ABI OBJDIR=$OBJDIR TVS=$TOOLCHAIN_VERSION
    fail_panic "Couldn't build $TYPE libcrystax"
}

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="crystax-libs-$ABI.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? = 0 ]; then
            DO_BUILD_PACKAGE="no"
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_crystax_libs_for_abi $ABI "$BUILD_DIR/$ABI/shared" "shared"
        build_crystax_libs_for_abi $ABI "$BUILD_DIR/$ABI/static" "static"
    fi
done

if [ "$PATCH_SYSROOT" = "yes" ]; then
    $NDK_DIR/$CRYSTAX_SUBDIR/bin/patch-sysroot --libraries --fast-copy
    fail_panic "Couldn't patch sysroot with CrystaX libraries"
fi

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    for ABI in $BUILT_ABIS; do
        FILES=""
        for MLIB in $($NDK_DIR/$CRYSTAX_SUBDIR/bin/config --multilibs --abi=$ABI); do
            LIBPATH=$($NDK_DIR/$CRYSTAX_SUBDIR/bin/config --libpath --abi=$ABI --multilib=$MLIB)
            for LIB in libcrystax.a libcrystax.so; do
                FILES="$FILES $CRYSTAX_SUBDIR/$LIBPATH/$LIB"
            done
        done
        PACKAGE_NAME="crystax-libs-$ABI.tar.xz"
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
