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
"Rebuild GNUstep libobjc2 for the Android NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$GNUSTEP_OBJC2_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>."

NDK_DIR=
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build."

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir."

ABIS="$PREBUILT_ABIS"
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs."

TOOLCHAIN_VERSION=clang${DEFAULT_LLVM_VERSION}
register_var_option "--toolchain-version=<ver>" TOOLCHAIN_VERSION "Specify toolchain version"

register_jobs_option

extract_parameters "$@"

LIBOBJC2_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$LIBOBJC2_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the GNUstep libobjc2 source tree. See --help" 1>&2
    exit 1
fi

if [ ! -d "$LIBOBJC2_SRCDIR" ]; then
    echo "ERROR: No such directory: '$LIBOBJC2_SRCDIR'" 1>&2
    exit 1
fi

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
    BUILD_DIR=$NDK_TMPDIR/build-gnustep-libobjc2
else
    BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# $1: ABI
# $2: build directory
build_libobjc2_for_abi ()
{
    local ABI=$1
    local BUILDDIR="$2"

    mkdir -p "$BUILDDIR"
    fail_panic "Couldn't create GNUstep libobjc2 build directory for $ABI"

    run cd $BUILDDIR || exit 1

    dump "=== Building GNUstep libobjc2 for $ABI"
    run cmake \
        -DCMAKE_TOOLCHAIN_FILE="$NDK_DIR/cmake/toolchain.cmake" \
        -DANDROID_ABI=$ABI \
        -DANDROID_TOOLCHAIN_VERSION=$TOOLCHAIN_VERSION \
        $LIBOBJC2_SRCDIR
    fail_panic "Couldn't generate GNUstep libobjc2 Makefile for $ABI"

    run make -j$NUM_JOBS VERBOSE=1
    fail_panic "Couldn't build GNUstep libobjc2 for $ABI"

    if [ -z "$OBJC2_HEADERS_INSTALLED" ]; then
        run rm -Rf $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/include
        run mkdir -p $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/include
        fail_panic "Can't create directory for headers"
        run cp -r $LIBOBJC2_SRCDIR/objc $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/include/
        fail_panic "Can't install headers"
        OBJC2_HEADERS_INSTALLED=yes
        export OBJC2_HEADERS_INSTALLED
    fi

    run rm -Rf $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/libs/$ABI
    run mkdir -p $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/libs/$ABI
    fail_panic "Can't create directory for $ABI libraries"

    for f in libobjc.so; do
        run cp $BUILDDIR/$f $NDK_DIR/$GNUSTEP_OBJC2_SUBDIR/libs/$ABI
        fail_panic "Can't install $ABI libraries"
    done
}

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="gnustep-objc2-headers.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_NAME" "$PACKAGE_DIR" no_exit
    if [ $? -eq 0 ]; then
        OBJC2_HEADERS_NEED_PACKAGE=no
    else
        OBJC2_HEADERS_NEED_PACKAGE=yes
    fi
fi

OBJC2_HEADERS_INSTALLED=

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="gnustep-objc2-libs-$ABI.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? = 0 ]; then
            if [ "$OBJC2_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_libobjc2_for_abi $ABI "$BUILD_DIR/$ABI"
    fi
done

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    if [ "$OBJC2_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES=$GNUSTEP_OBJC2_SUBDIR/include
        PACKAGE_NAME="gnustep-objc2-headers.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package GNUstep objc2 headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES=""
        for LIB in libobjc.so; do
            FILES="$FILES $GNUSTEP_OBJC2_SUBDIR/libs/$ABI/$LIB"
        done
        PACKAGE_NAME="gnustep-objc2-libs-$ABI.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        log "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI GNUstep objc2 binaries!"
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
