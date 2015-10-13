#!/bin/bash

# Copyright (c) 2011-2015 CrystaX .NET.
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
"Rebuild sqlite3 libraries for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$SQLITE3_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build"

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir"

ABIS=$PREBUILT_ABIS
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs"

register_jobs_option

extract_parameters "$@"

SQLITE3_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$SQLITE3_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the sqlite3 source tree. See --help"
    exit 1
fi

if [ ! -d "$SQLITE3_SRCDIR" ]; then
    echo "ERROR: No such directory: '$SQLITE3_SRCDIR'"
    exit 1
fi

SQLITE3_SRCDIR=$(cd $SQLITE3_SRCDIR && pwd)

SQLITE3_DSTDIR=$NDK_DIR/$SQLITE3_SUBDIR
mkdir -p $SQLITE3_DSTDIR
fail_panic "Can't create sqlite3 destination directory: $SQLITE3_DSTDIR"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-sqlite3
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# $1: ABI
# $2: build directory
# $3: type (static or shared)
build_sqlite3_for_abi ()
{
    local ABI="$1"
    local BUILDDIR="$2"
    local TYPE="$3"

    dump "Building sqlite3 $ABI $TYPE libraries"

    run mkdir -p $BUILDDIR/jni
    fail_panic "Can't create $BUILDDIR/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := sqlite3'
        echo "LOCAL_SRC_FILES := $SQLITE3_SRCDIR/sqlite3.c"
        echo "LOCAL_INCLUDES := $SQLITE3_SRCDIR/"
        echo 'LOCAL_CFLAGS := -Wall -Wno-unused -Wno-multichar -Wstrict-aliasing=2 -Werror'
        case $ABI in
            x86|x86_64|arm64-v8a)
                echo 'LOCAL_CFLAGS += -Wno-strict-aliasing'
                ;;
        esac
        echo 'LOCAL_CFLAGS += -fno-exceptions -fmessage-length=0'
        echo 'LOCAL_CFLAGS += -DSQLITE_THREADSAFE=1'
        echo "include \$(BUILD_$(echo $TYPE | tr '[a-z]' '[A-Z]')_LIBRARY)"
    } | cat >$BUILDDIR/jni/Android.mk
    fail_panic "Can't generate Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR APP_ABI=$ABI V=1
    fail_panic "Can't build sqlite3 $ABI $TYPE library"

    if [ "$SQLITE3_HEADERS_INSTALLED" != "yes" ]; then
        log "Install sqlite3 headers into $SQLITE3_DSTDIR"
        run rm -Rf $SQLITE3_DSTDIR/include
        run mkdir -p $SQLITE3_DSTDIR/include && \
        run cp -p $SQLITE3_SRCDIR/*.h $SQLITE3_DSTDIR/include
        fail_panic "Can't install sqlite3 headers"
        SQLITE3_HEADERS_INSTALLED=yes
        export SQLITE3_HEADERS_INSTALLED
    fi

    log "Install sqlite3 $TYPE $ABI libraries into $SQLITE3_DSTDIR"
    run mkdir -p $SQLITE3_DSTDIR/libs/$ABI
    fail_panic "Can't create $SQLITE3_DSTDIR/libs/$ABI"

    local LIBSUFFIX
    if [ "$TYPE" = "shared" ]; then
        LIBSUFFIX=so
    else
        LIBSUFFIX=a
    fi

    local OBJDIR=$BUILDDIR/obj/local/$ABI

    run cp -fpH $OBJDIR/libsqlite3.$LIBSUFFIX $SQLITE3_DSTDIR/libs/$ABI
    fail_panic "Can't install sqlite3 $TYPE $ABI library"
}

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="sqlite3-build-files.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        SQLITE3_BUILD_FILES_NEED_PACKAGE=no
    else
        SQLITE3_BUILD_FILES_NEED_PACKAGE=yes
    fi

    PACKAGE_NAME="sqlite3-headers.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        SQLITE3_HEADERS_NEED_PACKAGE=no
    else
        SQLITE3_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="sqlite3-libs-$ABI.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$SQLITE3_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        for TYPE in static shared; do
            build_sqlite3_for_abi $ABI "$BUILD_DIR/$ABI/$TYPE" "$TYPE"
        done
    fi
done

log "Generating $SQLITE3_DSTDIR/Android.mk"
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

        echo ''
        echo 'include $(CLEAR_VARS)'
        echo "LOCAL_MODULE := sqlite3_${type}"
        echo "LOCAL_SRC_FILES := libs/\$(TARGET_ARCH_ABI)/libsqlite3.${suffix}"
        echo 'LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include'
        echo "include \$(PREBUILT_$(echo $type | tr '[a-z]' '[A-Z]')_LIBRARY)"
    done
} | cat >$SQLITE3_DSTDIR/Android.mk

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    if [ "$SQLITE3_BUILD_FILES_NEED_PACKAGE" = "yes" ]; then
        FILES=""
        for F in Android.mk; do
            FILES="$FILES $SQLITE3_SUBDIR/$F"
        done
        PACKAGE_NAME="sqlite3-build-files.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package sqlite3 build files!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    if [ "$SQLITE3_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$SQLITE3_SUBDIR/include"
        PACKAGE_NAME="sqlite3-headers.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package sqlite3 headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES="$SQLITE3_SUBDIR/libs/$ABI"
        PACKAGE_NAME="sqlite3-libs-$ABI.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI sqlite3 binaries!"
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
