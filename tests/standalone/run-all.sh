#!/bin/bash
#
# Copyright (C) 2013 The Android Open Source Project
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

# 
# Copyright (c) 2013 Dmitry Moskalchuk <dm@crystax.net>.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#    1. Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation
# are those of the authors and should not be interpreted as representing
# official policies, either expressed or implied, of Dmitry Moskalchuk.
# 

# This simple script will create standalone toolchains for all supported
# GCC versions and for all supported CPU architectures and then it will
# run all required tests.
#

MSTS=./build/tools/make-standalone-toolchain.sh
RS=./tests/standalone/run.sh
IBD=/tmp/ndk-$USER
GCC_VERS="4.7 4.6 4.4.3"
ARM_NM="arm-linux-androideabi"
MIPS_NM="mipsel-linux-android"
X86_NM="x86"
ARM_GCC_NM=$ARM_NM-gcc
MIPS_GCC_NM=$MIPS_NM-gcc
X86_GCC_NM=i686-linux-android-gcc

for ver in $GCC_VERS
do
    echo ===== Creating toolchains for version $ver =====
    echo "== ARCH: armeabi/armeabi-v7a"
    $MSTS --toolchain="$ARM_NM-$ver"  --arch=arm  --install-dir=$IBD/$ARM_NM-$ver
    echo "== ARCH: mips"
    $MSTS --toolchain="$MIPS_NM-$ver" --arch=mips --install-dir=$IBD/$MIPS_NM-$ver
    echo "== ARCH: x86"
    $MSTS --toolchain="$X86_NM-$ver"  --arch=x86  --install-dir=$IBD/$X86_NM-$ver
done

for ver in $GCC_VERS
do
    echo ===== Running tests for toolchains version $ver =====
    echo "== ARCH: armeabi-v7a"
    $RS --no-sysroot --abi=armeabi     --prefix=$IBD/$ARM_NM-$ver/bin/$ARM_GCC_NM
    echo "== ARCH: armeabi-v7a"
    $RS --no-sysroot --abi=armeabi-v7a --prefix=$IBD/$ARM_NM-$ver/bin/$ARM_GCC_NM
    echo "== ARCH: mips"
    $RS --no-sysroot --abi=mips        --prefix=$IBD/$MIPS_NM-$ver/bin/$MIPS_GCC_NM
    echo "== ARCH: x86"
    $RS --no-sysroot --abi=x86         --prefix=$IBD/$X86_NM-$ver/bin/$X86_GCC_NM
done

echo Cleaning up...
for ver in $GCC_VERS
do
    echo Removing toolchains for version $ver
    rm -rf $IBD/$ARM_NM-$ver
    rm -rf $IBD/$MIPS_NM-$ver
    rm -rf $IBD/$X86_NM-$ver
done
