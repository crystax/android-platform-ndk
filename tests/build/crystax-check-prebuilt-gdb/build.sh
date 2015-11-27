#!/bin/bash

# Copyright (c) 2015 CrystaX .NET.
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

echo ""
echo "================================================================="
echo "===> Checking prebuilt GDB <==="
echo "================================================================="
echo ""

EXE=
HOSTOS=$(uname | tr '[:upper:]' '[:lower:]')
if [[ $HOSTOS == cygwin*  || $HOSTOS == mingw* ]]; then
    HOSTOS=windows
    EXE=".exe"
fi

FILE_DIR=$(dirname $0)
NDK_DIR=$(cd $FILE_DIR/../../.. && pwd)
NDK_BUILDTOOLS_PATH="$NDK_DIR/build/tools"

#. $NDK_DIR/build/tools/dev-defaults.sh
#. $NDK_DIR/build/tools/prebuilt-common.sh

run()
{
    echo "## COMMAND: $@"
    "$@"
}

for toolchain in $(ls $NDK_DIR/toolchains | grep -v 'clang\|llvm\|renderscript'); do
    echo "===> Checking GDB for toolchain $toolchain"
    for hostos in $(ls $NDK_DIR/toolchains/$toolchain/prebuilt); do
        # skip GDB built for OS different from host OS
        if [ "${hostos##$HOSTOS}" == "$hostos" ]; then
            echo "Skipping GDB built for $hostos"
            continue
        fi
        gdb=$(ls $NDK_DIR/toolchains/$toolchain/prebuilt/$hostos/bin/*-gdb$EXE)
        run $gdb --version
        if [ $? -ne 0 ]; then
            echo "ERROR: GDB for toolchain $toolchain and host OS $hostos failed to start" 1>&2
            exit 1
        fi
        echo ""
        echo "===> OK"
    done
done
