#!/bin/bash
#
# Copyright (C) 2011, 2014, 2015 The Android Open Source Project
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
# Rebuild all target-specific prebuilts
#

PROGDIR=$(dirname $0)
. $PROGDIR/prebuilt-common.sh

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "NDK installation directory"

ARCHS="$DEFAULT_ARCHS"
register_var_option "--arch=<list>" ARCHS "List of target archs to build for"

NO_GEN_PLATFORMS=
register_var_option "--no-gen-platforms" NO_GEN_PLATFORMS "Don't generate platforms/ directory, use existing one"

GCC_VERSION_LIST="default" # it's arch defined by default so use default keyword
register_var_option "--gcc-version-list=<vers>" GCC_VERSION_LIST "GCC version list (libgnustl should be built per each GCC version)"

LLVM_VERSION_LIST=$(spaces_to_commas $DEFAULT_LLVM_VERSION_LIST)
register_var_option "--llvm-version-list=<vers>" LLVM_VERSION_LIST "LLVM version list"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Package toolchain into this directory"

VISIBLE_LIBGNUSTL_STATIC=
register_var_option "--visible-libgnustl-static" VISIBLE_LIBGNUSTL_STATIC "Do not use hidden visibility for libgnustl_static.a"

register_jobs_option

register_try64_option

PROGRAM_PARAMETERS="<toolchain-src-dir>"
PROGRAM_DESCRIPTION="This script can be used to rebuild all the target NDK prebuilts at once."

extract_parameters "$@"

# Pickup one GCC_VERSION for the cases where we want only one build
# That's actually all cases except libgnustl where we are building for each GCC version
GCC_VERSION=$DEFAULT_GCC_VERSION
if [ "$GCC_VERSION_LIST" != "default" ]; then
   GCC_VERSIONS=$(commas_to_spaces $GCC_VERSION_LIST)
   GCC_VERSION=${GCC_VERSIONS%% *}
fi

LLVM_VERSION_LIST=$(commas_to_spaces $LLVM_VERSION_LIST)

BUILD_TOOLCHAIN="--gcc-version=$GCC_VERSION"

# Check toolchain source path
SRC_DIR="$PARAMETERS"
check_toolchain_src_dir "$SRC_DIR"
SRC_DIR=`cd $SRC_DIR; pwd`

VENDOR_SRC_DIR=$(cd $SRC_DIR/../vendor && pwd)

# Now we can do the build
BUILDTOOLS=$ANDROID_NDK_ROOT/build/tools

dump "Building platforms and samples..."
PACKAGE_FLAGS=
if [ "$PACKAGE_DIR" ]; then
    PACKAGE_FLAGS="--package-dir=$PACKAGE_DIR"
fi

if [ -z "$NO_GEN_PLATFORMS" ]; then
    echo "Preparing the build..."
    PLATFORMS_BUILD_TOOLCHAIN=
    if [ ! -z "$GCC_VERSION" ]; then
	PLATFORMS_BUILD_TOOLCHAIN="--gcc-version=$GCC_VERSION"
    fi
    run $BUILDTOOLS/gen-platforms.sh --samples --fast-copy --dst-dir=$NDK_DIR --ndk-dir=$NDK_DIR --arch=$(spaces_to_commas $ARCHS) $PACKAGE_FLAGS $PLATFORMS_BUILD_TOOLCHAIN
    fail_panic "Could not generate platforms and samples directores!"
else
    if [ ! -d "$NDK_DIR/platforms" ]; then
        echo "ERROR: --no-gen-platforms used but directory missing: $NDK_DIR/platforms"
        exit 1
    fi
fi

ARCHS=$(commas_to_spaces $ARCHS)

FLAGS=
if [ "$DRYRUN" = "yes" ]; then
    FLAGS=$FLAGS" --dryrun"
fi
if [ "$VERBOSE" = "yes" ]; then
    FLAGS=$FLAGS" --verbose"
fi
if [ "$PACKAGE_DIR" ]; then
    mkdir -p "$PACKAGE_DIR"
    fail_panic "Could not create package directory: $PACKAGE_DIR"
    FLAGS=$FLAGS" --package-dir=\"$PACKAGE_DIR\""
fi
if [ "$TRY64" = "yes" ]; then
    FLAGS=$FLAGS" --try-64"
fi
FLAGS=$FLAGS" -j$NUM_JOBS"

# First, gdbserver
for ARCH in $ARCHS; do
    if [ -z "$GCC_VERSION" ]; then
       GDB_TOOLCHAIN=$(get_default_toolchain_name_for_arch $ARCH)
    else
       GDB_TOOLCHAIN=$(get_toolchain_name_for_arch $ARCH $GCC_VERSION)
    fi
    GDB_VERSION="--gdb-version="$(get_default_gdb_version_for_gcc $GDB_TOOLCHAIN)
    dump "Building $GDB_TOOLCHAIN gdbserver binaries..."
    run $BUILDTOOLS/build-gdbserver.sh "$SRC_DIR" "$NDK_DIR" "$GDB_TOOLCHAIN" "$GDB_VERSION" $FLAGS --platform=android-21
    fail_panic "Could not build $GDB_TOOLCHAIN gdb-server!"
done

FLAGS=$FLAGS" --ndk-dir=\"$NDK_DIR\""
ABIS=$(convert_archs_to_abis $ARCHS)

dump "Building $ABIS libcrystax binaries..."
run $BUILDTOOLS/build-crystax.sh --abis="$ABIS" --patch-sysroot $FLAGS
fail_panic "Could not build libcrystax!"

dump "Building $ABIS compiler-rt binaries..."
run $BUILDTOOLS/build-compiler-rt.sh --abis="$ABIS" $FLAGS --src-dir="$SRC_DIR/llvm-$DEFAULT_LLVM_VERSION/compiler-rt" $BUILD_TOOLCHAIN --llvm-version=$DEFAULT_LLVM_VERSION
fail_panic "Could not build compiler-rt!"

dump "Building $ABIS gabi++ binaries..."
run $BUILDTOOLS/build-cxx-stl.sh --stl=gabi++ --abis="$ABIS" $FLAGS --with-debug-info $BUILD_TOOLCHAIN
fail_panic "Could not build gabi++ with debug info!"

dump "Building $ABIS stlport binaries..."
run $BUILDTOOLS/build-cxx-stl.sh --stl=stlport --abis="$ABIS" $FLAGS --with-debug-info $BUILD_TOOLCHAIN
fail_panic "Could not build stlport with debug info!"

for VERSION in $LLVM_VERSION_LIST; do
    dump "Building $ABIS LLVM libc++ $VERSION binaries... with libc++abi"
    run $BUILDTOOLS/build-cxx-stl.sh --stl=libc++-libc++abi --abis="$ABIS" $FLAGS --with-debug-info --llvm-version=$VERSION
    fail_panic "Could not build LLVM libc++ $VERSION!"

    # workaround issues in libc++/libc++abi for x86 and mips
    #for abi in $(commas_to_spaces $ABIS); do
    #    case $abi in
    #        x86|x86_64|mips|mips32r6|mips64)
    #            dump "Rebuilding $abi libc++ binaries... with gabi++"
    #            run $BUILDTOOLS/build-cxx-stl.sh --stl=libc++-gabi++ --abis=$abi $FLAGS --with-debug-info --llvm-version=$VERSION
    #    esac
    #done
done

for VERSION in $(commas_to_spaces $GCC_VERSION_LIST); do
    dump "Building $ABIS GNU libobjc $VERSION binaries..."
    run $BUILDTOOLS/build-gnu-libobjc.sh $FLAGS --abis="$ABIS" --gcc-version-list=$VERSION "$SRC_DIR"
    fail_panic "Could not build GNU libobjc $VERSION!"
done

dump "Building $ABIS Objective-C v2 runtime..."
run $BUILDTOOLS/build-gnustep-libobjc2.sh $FLAGS --abis="$ABIS" $VENDOR_SRC_DIR/libobjc2
fail_panic "Could not build Objective-C v2 runtime"

dump "Building $ABIS Cocotron frameworks..."
run $BUILDTOOLS/build-cocotron.sh $FLAGS --abis="$ABIS" $VENDOR_SRC_DIR/cocotron
fail_panic "Could not build Cocotron frameworks"

if [ ! -z $VISIBLE_LIBGNUSTL_STATIC ]; then
    GNUSTL_STATIC_VIS_FLAG=--visible-libgnustl-static
fi

if [ ! -z "$GCC_VERSION_LIST" ]; then
    if [ "$GCC_VERSION_LIST" = "default" ]; then
        STDCXX_GCC_VERSIONS="$DEFAULT_GCC_VERSION_LIST"
    else
        STDCXX_GCC_VERSIONS="$GCC_VERSION_LIST"
    fi
    for VERSION in $(commas_to_spaces $STDCXX_GCC_VERSIONS); do
        dump "Building $ABIS GNU libstdc++ $VERSION binaries..."
        run $BUILDTOOLS/build-gnu-libstdc++.sh --abis="$ABIS" $FLAGS $GNUSTL_STATIC_VIS_FLAG "$SRC_DIR" \
            --with-debug-info --gcc-version-list=$VERSION
        fail_panic "Could not build GNU libstdc++ $VERSION!"
    done
fi

dump "Building $ABIS sqlite3 binaries..."
run $BUILDTOOLS/build-sqlite3.sh $FLAGS --abis="$ABIS" $VENDOR_SRC_DIR/sqlite3
fail_panic "Could not build sqlite3"

for LIBPNG_VERSION in $LIBPNG_VERSIONS; do
    dump "Building $ABIS libpng-$LIBPNG_VERSION binaries..."
    run $BUILDTOOLS/build-libpng.sh $FLAGS --abis="$ABIS" --version=$LIBPNG_VERSION $VENDOR_SRC_DIR/libpng
    fail_panic "Could not build libpng-$LIBPNG_VERSION"
done

for LIBJPEG_VERSION in $LIBJPEG_VERSIONS; do
    dump "Building $ABIS libjpeg-$LIBJPEG_VERSION binaries..."
    run $BUILDTOOLS/build-libjpeg.sh $FLAGS --abis="$ABIS" --version=$LIBJPEG_VERSION $VENDOR_SRC_DIR/libjpeg
    fail_panic "Could not build libjpeg-$LIBJPEG_VERSION"
done

for LIBJPEGTURBO_VERSION in $LIBJPEGTURBO_VERSIONS; do
    dump "Building $ABIS libjpeg-turbo-$LIBJPEGTURBO_VERSION binaries..."
    run $BUILDTOOLS/build-libjpeg-turbo.sh $FLAGS --abis="$ABIS" --version=$LIBJPEGTURBO_VERSION $VENDOR_SRC_DIR/libjpeg-turbo
    fail_panic "Could not build libjpeg-turbo-$LIBJPEGTURBO_VERSION"
done

for LIBTIFF_VERSION in $LIBTIFF_VERSIONS; do
    dump "Building $ABIS libtiff-$LIBTIFF_VERSION binaries..."
    run $BUILDTOOLS/build-libtiff.sh $FLAGS --abis="$ABIS" --version=$LIBTIFF_VERSION $VENDOR_SRC_DIR/libtiff
    fail_panic "Could not build libtiff-$LIBTIFF_VERSION"
done

ICU_VERSION=$(echo $ICU_VERSIONS | tr ' ' '\n' | grep -v '^$' | tail -n 1)
dump "Building $ABIS ICU-$ICU_VERSION binaries..."
run $BUILDTOOLS/build-icu.sh $FLAGS --version=$ICU_VERSION --abis="$ABIS" $VENDOR_SRC_DIR/icu
fail_panic "Could not build ICU-$ICU_VERSION!"

for VERSION in $BOOST_VERSIONS; do
    dump "Building $ABIS boost-$VERSION binaries..."
    run $BUILDTOOLS/build-boost.sh $FLAGS --version=$VERSION --abis="$ABIS" $VENDOR_SRC_DIR/boost
    fail_panic "Could not build Boost-$VERSION!"

    dump "Building $ABIS boost+icu-$VERSION binaries..."
    run $BUILDTOOLS/build-boost.sh $FLAGS --version=$VERSION --abis="$ABIS" --with-icu=$ICU_VERSION $VENDOR_SRC_DIR/boost
    fail_panic "Could not build Boost+ICU-$VERSION!"
done

dump "Cleanup sysroot folders..."
run find $NDK_DIR/platforms -name libcrystax.a -delete
run find $NDK_DIR/platforms -name libcrystax.so -delete

if [ "$PACKAGE_DIR" ]; then
    dump "Done, see $PACKAGE_DIR"
else
    dump "Done"
fi

exit 0
