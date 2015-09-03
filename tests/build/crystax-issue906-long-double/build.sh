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

. $NDK/build/tools/dev-defaults.sh

HOST_OS=$(uname -s | tr '[A-Z]' '[a-z]')
case $HOST_OS in
    cygwin*|mingw*)
        HOST_OS=windows
esac

HOST_ARCH=""
HOST_ARCH2=""

case $HOST_OS in
    darwin|linux|windows)
        HOST_ARCH=$(uname -m)
        HOST_ARCH2=
        case $HOST_ARCH in
            i?86)
                HOST_ARCH=x86
                ;;
            x86_64)
                if [ "$HOST_OS" = "linux" ]; then
                    if file -b /bin/ls | grep -q 32-bit; then
                        HOST_ARCH=x86
                    else
                        HOST_ARCH2=x86
                    fi
                else
                    HOST_ARCH2=x86
                fi
                ;;
            *)
                echo "ERROR: Unsupported host CPU architecture: '$HOST_ARCH'" 1>&2
                exit 1
        esac
        HOST_TAG=${HOST_OS}-${HOST_ARCH}
        test -n "$HOST_ARCH2" && HOST_TAG2=${HOST_OS}-${HOST_ARCH2}
        ;;
    *)
        echo "ERROR: Unsupported host OS: '$HOST_OS'" 1>&2
        exit 1
        ;;
esac

if [ "$HOST_OS" = "windows" -a "$HOST_ARCH" = "x86" ]; then
    HOST_TAG=$HOST_OS
else
    HOST_TAG=${HOST_OS}-${HOST_ARCH}
fi

HOST_TAG2=""
if [ -n "$HOST_ARCH2" ]; then
    if [ "$HOST_OS" = "windows" -a "$HOST_ARCH2" = "x86" ]; then
        HOST_TAG2=$HOST_OS
    else
        HOST_TAG2=${HOST_OS}-${HOST_ARCH2}
    fi
fi

$NDK/ndk-build -B || exit 1

for ABI in $(ls -1 libs | sort); do
    echo "Check $ABI ..."

    case $ABI in
        armeabi*)
            ARCH=arm
            ;;
        arm64-v8a)
            ARCH=arm64
            ;;
        *)
            ARCH=$ABI
    esac

    TOOLCHAIN_NAME=$(get_default_toolchain_name_for_arch $ARCH)
    TOOLCHAIN_PREFIX=$(get_default_toolchain_prefix_for_arch $ARCH)

    for tag in $HOST_TAG $HOST_TAG2; do
        NM=$NDK/toolchains/$TOOLCHAIN_NAME/prebuilt/$tag/bin/${TOOLCHAIN_PREFIX}-nm
        echo "Probing for ${NM}..."
        if [ -x $NM ]; then
            echo "Found: $NM"
            break
        fi
    done

    if [ ! -x "$NM" ]; then
        echo "ERROR: Can't find $ARCH nm" 1>&2
        exit 1
    fi

    case $ABI in
        x86_64)
            TYPEINFO="_ZTIg"
            ;;
        *)
            TYPEINFO="_ZTIe"
    esac

    $NM -D libs/$ABI/libtest.so | grep -q "^ *U $TYPEINFO\>"
    if [ $? -ne 0 ]; then
        echo "ERROR: $ABI library don't contain 'typeinfo for long double' reference" 1>&2
        exit 1
    fi

    echo "OK"
done

echo "Done!"
