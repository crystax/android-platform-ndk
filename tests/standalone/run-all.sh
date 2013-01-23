#!/bin/bash
#
# Copyright (C) 2012 The Android Open Source Project
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

# This simple script will create standalone toolchains for all supported
# GCC version and for all supported CPU architectures and then it will
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
