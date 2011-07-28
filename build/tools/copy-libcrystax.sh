#!/bin/sh
#
# Copyright (C) 2010 The Android Open Source Project
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
#  This shell script is used to copy the prebuilt libcrystax binaries
#  from sources directory to toolchain directory
#

. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<ndk-dir> <toolchain-dir>"
PROGRAM_DESCRIPTION="\
This program is used to copy libcrystax binaries from a
sources directory into toolchain directory.
It will copy the files (headers and libraries) under <ndk-dir>/$CRYSTAX_SUBDIR
unless you use the --src-dir option.
"

NDK_DIR="$ANDROID_NDK_ROOT"
register_var_option "--ndk-dir=<path>" NDK_DIR "Source NDK installation."

SRC_DIR=
register_var_option "--src-dir=<path>" SRC_DIR "Alternate installation location."

CLEAN_NDK=no
register_var_option "--clean-ndk" CLEAN_NDK "Remove binaries from NDK installation."

TOOLCHAIN=arm-linux-androideabi-4.4.3
register_var_option "--toolchain=<name>" TOOLCHAIN "Specify toolchain name."

extract_parameters "$@"

# Set HOST_TAG to linux-x86 on 64-bit Linux systems
force_32bit_binaries

set_parameters ()
{
    NDK_DIR="$1"
    TOOLCHAIN_DIR="$2"

    # Check source directory
    #
    if [ -z "$TOOLCHAIN_DIR" ] ; then
        echo "ERROR: Missing toolchain directory parameter. See --help for details."
        exit 1
    fi

    TOOLCHAIN_DIR2="$TOOLCHAIN_DIR/toolchains/$TOOLCHAIN/prebuilt/$HOST_TAG"
    if [ -d "$TOOLCHAIN_DIR2" ] ; then
        TOOLCHAIN_DIR="$TOOLCHAIN_DIR2"
        log "Auto-detecting toolchain installation: $TOOLCHAIN_DIR"
    fi

    if [ ! -d "$TOOLCHAIN_DIR/bin" -o ! -d "$TOOLCHAIN_DIR/lib" ] ; then
        echo "ERROR: Directory does not point to toolchain: $TOOLCHAIN_DIR"
        exit 1
    fi

    log "Using toolchain directory: $TOOLCHAIN_DIR"

    # Check NDK installation directory
    #
    if [ -z "$NDK_DIR" ] ; then
        echo "ERROR: Missing NDK directory parameter. See --help for details."
        exit 1
    fi

    if [ ! -d "$NDK_DIR" ] ; then
        echo "ERROR: Not a valid directory: $NDK_DIR"
        exit 1
    fi

    log "Using NDK directory: $NDK_DIR"
}

set_parameters $PARAMETERS

parse_toolchain_name

# Determine output directory
if [ -z "$SRC_DIR" ] ; then
    SRC_DIR="$NDK_DIR/$CRYSTAX_SUBDIR"
    log "Using default output directory: $SRC_DIR"
    mkdir -p "$SRC_DIR"
fi

if [ ! -d "$SRC_DIR" ] ; then
    panic "Directory does not exist: $SRC_DIR"
fi

ABI_STL="$TOOLCHAIN_DIR/$ABI_CONFIGURE_TARGET"

SRC_INCLUDE="$SRC_DIR/include"
SRC_INCLUDE_ABI="$SRC_INCLUDE/$ABI_CONFIGURE_TARGET"
SRC_LIBS="$SRC_DIR/libs"
SRC_ABI="$SRC_DIR/$ABI_CONFIGURE_TARGET"

case "$ARCH" in
    arm)
        copy_file_list "$SRC_LIBS/armeabi" "$ABI_STL/lib" "libcrystax_*.*"
        copy_file_list "$SRC_LIBS/armeabi" "$ABI_STL/lib/thumb" "libcrystax_*.*"
        copy_file_list "$SRC_LIBS/armeabi-v7a" "$ABI_STL/lib/armv7-a" "libcrystax_*.*"
        copy_file_list "$SRC_LIBS/armeabi-v7a" "$ABI_STL/lib/armv7-a/thumb" "libcrystax_*.*"
        ;;
    x86)
        copy_file_list "$SRC_LIBS/x86" "$ABI_STL/lib" "libcrystax_*.*"
        ;;
    *)
        dump "ERROR: Unsupported NDK architecture!"
esac
