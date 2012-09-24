#!/bin/bash
#
# Copyright (C) 2011 The Android Open Source Project
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

BUILD_OUT=/tmp/ndk-$USER/build/target
OPTION_BUILD_OUT=
register_var_option "--build-out=<path>" OPTION_BUILD_OUT "Set temporary build directory"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "NDK installation directory"

ARCHS=$DEFAULT_ARCHS
register_var_option "--arch=<list>" ARCHS "List of target archs to build for"

GCC_VERSION_LIST=$DEFAULT_GCC_VERSION_LIST
register_var_option "--gcc-ver-list=<list>" GCC_VERSION_LIST "List of GCC versions to use for build"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Package toolchain into this directory"

SKIP_GEN_PLATFORMS=no
register_option "--skip-gen-platforms" do_skip_gen_platforms "Skip generation of platforms"
do_skip_gen_platforms ()
{
    SKIP_GEN_PLATFORMS=yes
}

SKIP_BUILD_GDBSERVER=no
register_option "--skip-build-gdbserver" do_skip_build_gdbserver "Skip build of gdbserver binaries"
do_skip_build_gdbserver ()
{
    SKIP_BUILD_GDBSERVER=yes
}

SKIP_BUILD_GABIXX=no
register_option "--skip-build-gabi" do_skip_build_gabixx "Skip build of gabi++ binaries"
do_skip_build_gabixx ()
{
    SKIP_BUILD_GABIXX=yes
}

SKIP_BUILD_CRYSTAX=no
register_option "--skip-build-crystax" do_skip_build_crystax "Skip build of crystax binaries"
do_skip_build_crystax ()
{
    SKIP_BUILD_CRYSTAX=yes
}

SKIP_BUILD_STLPORT=no
register_option "--skip-build-stlport" do_skip_build_stlport "Skip build of stlport binaries"
do_skip_build_stlport ()
{
    SKIP_BUILD_STLPORT=yes
}

SKIP_BUILD_GNUSTL=no
register_option "--skip-build-gnustl" do_skip_build_gnustl "Skip build of GNU libstdc++"
do_skip_build_gnustl ()
{
    SKIP_BUILD_GNUSTL=yes
}

register_jobs_option

PROGRAM_PARAMETERS="<toolchain-src-dir>"
PROGRAM_DESCRIPTION=\
"This script can be used to rebuild all the target NDK prebuilts at once.
You need to give it the path to the toolchain source directory, as
downloaded by the 'download-toolchain-sources.sh' dev-script."

extract_parameters "$@"

fix_option BUILD_OUT "$OPTION_BUILD_OUT" "build directory"
setup_default_log_file $BUILD_OUT/build.log

# Check toolchain source path
SRC_DIR="$PARAMETERS"
check_toolchain_src_dir "$SRC_DIR"

# Now we can do the build
BUILDTOOLS=$ANDROID_NDK_ROOT/build/tools

dump "Building platforms and samples..."
PACKAGE_FLAGS=
if [ "$PACKAGE_DIR" ]; then
    PACKAGE_FLAGS="--package-dir=$PACKAGE_DIR"
fi

if [ "$SKIP_GEN_PLATFORMS" != "yes" ]; then
    dump "Generating platforms..."
    run $BUILDTOOLS/gen-platforms.sh --samples --fast-copy --dst-dir=$NDK_DIR --ndk-dir=$NDK_DIR --arch=$(spaces_to_commas $ARCHS) $PACKAGE_FLAGS
    fail_panic "Could not generate platforms and samples directores!"
fi

ARCHS=$(commas_to_spaces $ARCHS)

FLAGS=
if [ "$VERBOSE" = "yes" ]; then
    FLAGS=$FLAGS" --verbose"
fi
if [ "$VERBOSE2" = "yes" ]; then
    FLAGS=$FLAGS" --verbose"
fi
if [ "$DRY_RUN" = "yes" ] ; then
    FLAGS=$FLAGS" --dry-run"
fi
if [ "$PACKAGE_DIR" ]; then
    mkdir -p "$PACKAGE_DIR"
    fail_panic "Could not create package directory: $PACKAGE_DIR"
    FLAGS=$FLAGS" --package-dir=\"$PACKAGE_DIR\""
fi
if [ -n "$XCODE_PATH" ]; then
    FLAGS=$FLAGS" --xcode=$XCODE_PATH"
fi
if [ -n "$OPTION_BUILD_OUT" ]; then
    FLAGS=$FLAGS" --build-out=$BUILD_OUT"
fi
FLAGS=$FLAGS" -j$NUM_JOBS"

if [ "$SKIP_BUILD_GDBSERVER" != "yes" ]; then
    # First, gdbserver
    for ARCH in $ARCHS; do
        GDB_TOOLCHAINS=$(get_default_toolchain_name_for_arch $ARCH)
        for GDB_TOOLCHAIN in $GDB_TOOLCHAINS; do
            dump "Building $GDB_TOOLCHAIN gdbserver binaries..."
            run $BUILDTOOLS/build-gdbserver.sh "$SRC_DIR" "$NDK_DIR" "$GDB_TOOLCHAIN" $FLAGS
            fail_panic "Could not build $GDB_TOOLCHAIN gdb-server!"
        done
    done
fi

FLAGS=$FLAGS" --ndk-dir=\"$NDK_DIR\""
ABIS=$(convert_archs_to_abis $ARCHS)

FLAGS=$FLAGS" --abis=$ABIS"

if [ "$SKIP_BUILD_GABIXX" != "yes" ]; then
    dump "Building $ABIS gabi++ binaries..."
    run $BUILDTOOLS/build-gabi++.sh $FLAGS
    fail_panic "Could not build gabi++!"
fi

if [ "$SKIP_BUILD_CRYSTAX" != "yes" ]; then
    dump "Building $ABIS crystax binaries..."
    run $BUILDTOOLS/build-crystax.sh $FLAGS
    fail_panic "Could not build crystax binaries!"
fi

if [ "$SKIP_BUILD_STLPORT" != "yes" ]; then
    dump "Building $ABIS stlport binaries..."
    run $BUILDTOOLS/build-stlport.sh $FLAGS
    fail_panic "Could not build stlport!"
fi

if [ "$SKIP_BUILD_GNUSTL" != "yes" ]; then
    dump "Building $ABIS gnustl binaries..."
    run $BUILDTOOLS/build-gnu-libstdc++.sh $FLAGS --gcc-ver-list=$(spaces_to_commas $GCC_VERSION_LIST) "$SRC_DIR"
    fail_panic "Could not build gnustl!"
fi

if [ "$SKIP_BUILD_GNUOBJC" != "yes" ]; then
    dump "Building $ABIS gnuobjc binaries..."
    run $BUILDTOOLS/build-gnu-libobjc.sh $FLAGS --gcc-ver-list=$(spaces_to_commas $GCC_VERSION_LIST) "$SRC_DIR"
    fail_panic "Could not build gnuobjc!"
fi

if [ "$PACKAGE_DIR" ]; then
    dump "Done, see $PACKAGE_DIR"
else
    dump "Done"
fi

exit 0
