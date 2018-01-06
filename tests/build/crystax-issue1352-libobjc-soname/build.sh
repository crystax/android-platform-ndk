#!/usr/bin/env bash

# Copyright (c) 2011-2016, 2018 CrystaX.
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

HOSTOS=$(uname | tr '[:upper:]' '[:lower:]')
case $HOSTOS in
    cygwin*|mingw*)
        echo "=== Skip this test on Windows"
        exit 0
esac

if [ -z "$NDK" ]; then
    NDK=$(cd $(dirname $0)/../../.. && pwd)
fi
ANDROID_NDK_ROOT="$NDK"
NDK_BUILDTOOLS_PATH="$NDK/build/tools"

source $NDK/build/tools/dev-defaults.sh || exit 1
source $NDK/build/tools/prebuilt-common.sh || exit 1

#LIBOBJC2_DIR=$NDK/sources/objc/gnustep-libobjc2
LIBOBJC2_VER=1.8.1
LIBOBJC2_DIR=$NDK/packages/libobjc2/$LIBOBJC2_VER

if [ ! -d "$LIBOBJC2_DIR" ]; then
    echo "*** ERROR: No $LIBOBJC2_DIR found!" 1>&2
    exit 1
fi

PROPER_SONAME=libobjc.so

for ARCH in $DEFAULT_ARCHS; do
    TCNAME=$(get_default_toolchain_name_for_arch $ARCH)
    TCPREFIX=$(get_default_toolchain_prefix_for_arch $ARCH)
    OBJDUMP=$NDK/toolchains/$TCNAME/prebuilt/$HOST_TAG/bin/${TCPREFIX}-objdump
    if [ ! -x "$OBJDUMP" ]; then
        echo "*** ERROR: Can't find proper objdump for $ARCH!" 1>&2
        exit 1
    fi

    for ABI in $(get_default_abis_for_arch $ARCH); do
        LIBOBJC2="$LIBOBJC2_DIR/libs/$ABI/libobjc.so"
        if [ ! -e "$LIBOBJC2" ]; then
            echo "*** ERROR: No GNUstep libobjc2 found for ABI $ABI: $LIBOBJC2" 1>&2
            exit 1
        fi

        SONAME=$($OBJDUMP -x $LIBOBJC2 | grep '^\s*SONAME\>' | awk '{print $2}')
        if [ -z "$SONAME" ]; then
            echo "*** ERROR: Can't find SONAME for $LIBOBJC2!" 1>&2
            exit 1
        fi

        if [ "$SONAME" != "$PROPER_SONAME" ]; then
            echo "*** ERROR: Wrong SONAME found in $LIBOBJC2: $SONAME (should be $PROPER_SONAME)" 1>&2
            exit 1
        fi
    done
done
