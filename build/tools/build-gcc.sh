#!/bin/bash
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
#  This shell script is used to rebuild the gcc and toolchain binaries
#  for the Android NDK.
#

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir> <ndk-dir> <toolchain>"

PROGRAM_DESCRIPTION=\
"Rebuild the gcc toolchain prebuilt binaries for the Android NDK.

Where <src-dir> is the location of toolchain sources, <ndk-dir> is
the top-level NDK installation path and <toolchain> is the name of
the toolchain to use (e.g. arm-linux-androideabi-4.4.3)."

RELEASE=`date +%Y%m%d`
BUILD_OUT=/tmp/ndk-$USER/build/toolchain
OPTION_BUILD_OUT=
register_var_option "--build-out=<path>" OPTION_BUILD_OUT "Set temporary build directory"

# Note: platform API level 9 or higher is needed for proper C++ support
OPTION_PLATFORM=
register_var_option "--platform=<name>"  OPTION_PLATFORM "Specify platform name"

OPTION_SYSROOT=
register_var_option "--sysroot=<path>"   OPTION_SYSROOT   "Specify sysroot directory directly"

GDB_VERSION=$(get_default_gdb_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_GDB_VERSION=
register_var_option "--gdb-version=<version>" OPTION_GDB_VERSION "Specify gdb version [$GDB_VERSION]"

BINUTILS_VERSION=$(get_default_binutils_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_BINUTILS_VERSION=
register_var_option "--binutils-version=<version>" OPTION_BINUTILS_VERSION "Specify binutils version [$BINUTILS_VERSION]"

GMP_VERSION=$(get_default_gmp_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_GMP_VERSION=
register_var_option "--gmp-version=<version>" OPTION_GMP_VERSION "Specify gmp version [$GMP_VERSION]"

MPFR_VERSION=$(get_default_mpfr_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_MPFR_VERSION=
register_var_option "--mpfr-version=<version>" OPTION_MPFR_VERSION "Specify mpfr version [$MPFR_VERSION]"

MPC_VERSION=$(get_default_mpc_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_MPC_VERSION=
register_var_option "--mpc-version=<version>" OPTION_MPC_VERSION "Specify mpc version [$MPC_VERSION]"

EXPAT_VERSION=$(get_default_expat_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_EXPAT_VERSION=
register_var_option "--expat-version=<version>" OPTION_EXPAT_VERSION "Specify expat version [$EXPAT_VERSION]"

CLOOG_PPL_VERSION=$(get_default_cloog_ppl_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_CLOOG_PPL_VERSION=
register_var_option "--cloog-ppl-version=<version>" OPTION_CLOOG_PPL_VERSION "Specify cloog-ppl version [$CLOOG_PPL_VERSION]"

PPL_VERSION=$(get_default_ppl_version_for_gcc $DEFAULT_GCC_VERSION)
OPTION_PPL_VERSION=
register_var_option "--ppl-version=<version>" OPTION_PPL_VERSION "Specify ppl version [$PPL_VERSION]"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Create archive tarball in specific directory"

register_jobs_option
register_mingw_option
register_try64_option

extract_parameters "$@"

fix_option BUILD_OUT "$OPTION_BUILD_OUT" "build directory"
setup_default_log_file $BUILD_OUT/config.log

set_parameters ()
{
    SRC_DIR="$1"
    NDK_DIR="$2"
    TOOLCHAIN="$3"

    # Check source directory
    #
    if [ -z "$SRC_DIR" ] ; then
        echo "ERROR: Missing source directory parameter. See --help for details."
        exit 1
    fi

    if [ ! -d "$SRC_DIR/gcc" ] ; then
        echo "ERROR: Source directory does not contain gcc sources: $SRC_DIR"
        exit 1
    fi

    log "Using source directory: $SRC_DIR"

    # Check NDK installation directory
    #
    if [ -z "$NDK_DIR" ] ; then
        echo "ERROR: Missing NDK directory parameter. See --help for details."
        exit 1
    fi

    if [ ! -d "$NDK_DIR" ] ; then
        mkdir -p $NDK_DIR
        if [ $? != 0 ] ; then
            echo "ERROR: Could not create target NDK installation path: $NDK_DIR"
            exit 1
        fi
    fi

    log "Using NDK directory: $NDK_DIR"

    # Check toolchain name
    #
    if [ -z "$TOOLCHAIN" ] ; then
        echo "ERROR: Missing toolchain name parameter. See --help for details."
        exit 1
    fi
}

set_parameters $PARAMETERS

prepare_target_build

parse_toolchain_name $TOOLCHAIN

fix_sysroot "$OPTION_SYSROOT"

check_toolchain_src_dir "$SRC_DIR"

if [ -z "$OPTION_GDB_VERSION" ]; then
    GDB_VERSION=$(get_default_gdb_version_for_gcc $GCC_VERSION)
else
    GDB_VERSION=$OPTION_GDB_VERSION
fi

if [ -z "$OPTION_BINUTILS_VERSION" ]; then
    BINUTILS_VERSION=$(get_default_binutils_version_for_gcc $GCC_VERSION)
else
    BINUTILS_VERSION=$OPTION_BINUTILS_VERSION
fi

if [ -z "$OPTION_GMP_VERSION" ]; then
    GMP_VERSION=$(get_default_gmp_version_for_gcc $GCC_VERSION)
else
    GMP_VERSION=$OPTION_GMP_VERSION
fi

if [ -z "$OPTION_MPFR_VERSION" ]; then
    MPFR_VERSION=$(get_default_mpfr_version_for_gcc $GCC_VERSION)
else
    MPFR_VERSION=$OPTION_MPFR_VERSION
fi

if [ -z "$OPTION_MPC_VERSION" ]; then
    MPC_VERSION=$(get_default_mpc_version_for_gcc $GCC_VERSION)
else
    MPC_VERSION=$OPTION_MPC_VERSION
fi

if [ -z "$OPTION_EXPAT_VERSION" ]; then
    EXPAT_VERSION=$(get_default_expat_version_for_gcc $GCC_VERSION)
else
    EXPAT_VERSION=$OPTION_EXPAT_VERSION
fi

if [ -z "$OPTION_CLOOG_PPL_VERSION" ]; then
    CLOOG_PPL_VERSION=$(get_default_cloog_ppl_version_for_gcc $GCC_VERSION)
else
    CLOOG_PPL_VERSION=$OPTION_CLOOG_PPL_VERSION
fi

if [ -z "$OPTION_PPL_VERSION" ]; then
    PPL_VERSION=$(get_default_ppl_version_for_gcc $GCC_VERSION)
else
    PPL_VERSION=$OPTION_PPL_VERSION
fi

if [ ! -d $SRC_DIR/gdb/gdb-$GDB_VERSION ] ; then
    echo "ERROR: Missing gdb sources: $SRC_DIR/gdb/gdb-$GDB_VERSION"
    echo "       Use --gdb-version=<version> to specify alternative."
    exit 1
fi

if [ ! -d $SRC_DIR/binutils/binutils-$BINUTILS_VERSION ] ; then
    echo "ERROR: Missing binutils sources: $SRC_DIR/binutils/binutils-$BINUTILS_VERSION"
    echo "       Use --binutils-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/gmp/gmp-$GMP_VERSION.tar.bz2 ] ; then
    echo "ERROR: Missing gmp sources: $SRC_DIR/gmp/gmp-$GMP_VERSION.tar.bz2"
    echo "       Use --gmp-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/mpfr/mpfr-$MPFR_VERSION.tar.bz2 ] ; then
    echo "ERROR: Missing mpfr sources: $SRC_DIR/mpfr/mpfr-$MPFR_VERSION.tar.bz2"
    echo "       Use --mpfr-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/mpc/mpc-$MPC_VERSION.tar.gz ] ; then
    echo "ERROR: Missing mpc sources: $SRC_DIR/mpc/mpc-$MPC_VERSION.tar.gz"
    echo "       Use --mpc-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/expat/expat-$EXPAT_VERSION.tar.gz ] ; then
    echo "ERROR: Missing expat sources: $SRC_DIR/expat/expat-$EXPAT_VERSION.tar.gz"
    echo "       Use --expat-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/cloog/cloog-ppl-$CLOOG_PPL_VERSION.tar.gz ] ; then
    echo "ERROR: Missing cloog-ppl sources: $SRC_DIR/cloog/cloog-ppl-$CLOOG_PPL_VERSION.tar.gz"
    echo "       Use --cloog-ppl-version=<version> to specify alternative."
    exit 1
fi

if [ ! -f $SRC_DIR/ppl/ppl-$PPL_VERSION.tar.bz2 ] ; then
    echo "ERROR: Missing ppl sources: $SRC_DIR/ppl/ppl-$PPL_VERSION.tar.bz2"
    echo "       Use --ppl-version=<version> to specify alternative."
    exit 1
fi

if [ "$PACKAGE_DIR" ]; then
    mkdir -p "$PACKAGE_DIR"
    fail_panic "Could not create package directory: $PACKAGE_DIR"
fi

set_toolchain_ndk $NDK_DIR $TOOLCHAIN

dump "Using C compiler: $CC"
dump "Using C++ compiler: $CXX"

# Location where the toolchain license files are
TOOLCHAIN_LICENSES=$ANDROID_NDK_ROOT/build/tools/toolchain-licenses

# Copy the sysroot to the installation prefix. This prevents the generated
# binaries from containing hard-coding host paths
TOOLCHAIN_SYSROOT=$TOOLCHAIN_PATH/sysroot
dump "Sysroot  : Copying: $SYSROOT --> $TOOLCHAIN_SYSROOT"
mkdir -p $TOOLCHAIN_SYSROOT && (cd $SYSROOT && tar ch *) | (cd $TOOLCHAIN_SYSROOT && tar x)
if [ $? != 0 ] ; then
    echo "Error while copying sysroot files. See $TMPLOG"
    exit 1
fi

ABI_LDFLAGS_FOR_TARGET=""

# configure the toolchain
#
dump "Configure: $TOOLCHAIN toolchain build"
# Old versions of the toolchain source packages placed the
# configure script at the top-level. Newer ones place it under
# the build directory though. Probe the file system to check
# this.
BUILD_SRCDIR=$SRC_DIR/build
if [ ! -d $BUILD_SRCDIR ] ; then
    BUILD_SRCDIR=$SRC_DIR
fi
rm -rf $BUILD_OUT

CRYSTAX_EMPTY_DIR=$ANDROID_NDK_ROOT/$CRYSTAX_SUBDIR/empty
ABI_LDFLAGS_FOR_TARGET=$ABI_LDFLAGS_FOR_TARGET" -L$CRYSTAX_EMPTY_DIR/$ARCH"

OLD_ABI="${ABI}"
export CC CXX
export CFLAGS_FOR_TARGET="$ABI_CFLAGS_FOR_TARGET"
export CXXFLAGS_FOR_TARGET="$ABI_CXXFLAGS_FOR_TARGET"
export LDFLAGS_FOR_TARGET="$ABI_LDFLAGS_FOR_TARGET"
# Needed to build a 32-bit gmp on 64-bit systems
export ABI=$HOST_GMP_ABI
export CFLAGS="$HOST_CFLAGS"
# -Wno-error is needed because our gdb-6.6 sources use -Werror by default
# and fail to build with recent GCC versions.
export CFLAGS=$CFLAGS" -Wno-error"
export LDFLAGS="$HOST_LDFLAGS"
mkdir -p $BUILD_OUT && cd $BUILD_OUT && run \
$BUILD_SRCDIR/configure --target=$ABI_CONFIGURE_TARGET \
                        --enable-initfini-array \
                        --host=$ABI_CONFIGURE_HOST \
                        --build=$ABI_CONFIGURE_BUILD \
                        --disable-nls \
                        --prefix=$TOOLCHAIN_PATH \
                        --with-sysroot=$TOOLCHAIN_SYSROOT \
                        --with-binutils-version=$BINUTILS_VERSION \
                        --with-mpfr-version=$MPFR_VERSION \
                        --with-gmp-version=$GMP_VERSION \
                        --with-gcc-version=$GCC_VERSION \
                        --with-gdb-version=$GDB_VERSION \
                        --with-mpc-version=$MPC_VERSION \
                        --with-expat-version=$EXPAT_VERSION \
                        --with-cloog-ppl-version=$CLOOG_PPL_VERSION \
                        --with-ppl-version=$PPL_VERSION \
                        $ABI_CONFIGURE_EXTRA_FLAGS
if [ $? != 0 ] ; then
    dump "Error while trying to configure toolchain build. See $TMPLOG"
    exit 1
fi
ABI="$OLD_ABI"
# build the toolchain
dump "Building : $TOOLCHAIN toolchain [this can take a long time]."
cd $BUILD_OUT &&
export CC CXX &&
export ABI=$HOST_GMP_ABI &&
run make -j$NUM_JOBS
if [ $? != 0 ] ; then
    # Unfortunately, there is a bug in the GCC build scripts that prevent
    # parallel mingw builds to work properly on some multi-core machines
    # (but not all, sounds like a race condition). Detect this and restart
    # in single-process mode!
    if [ "$MINGW" = "yes" ] ; then
        dump "Parallel mingw build failed - continuing in single process mode!"
        run make -j1
        if [ $? != 0 ] ; then
            echo "Error while building mingw toolchain. See $TMPLOG"
            exit 1
        fi
    else
        echo "Error while building toolchain. See $TMPLOG"
        exit 1
    fi
fi
ABI="$OLD_ABI"

# install the toolchain to its final location
dump "Install  : $TOOLCHAIN toolchain binaries."
cd $BUILD_OUT && run make install
if [ $? != 0 ] ; then
    echo "Error while installing toolchain. See $TMPLOG"
    exit 1
fi
# don't forget to copy the GPL and LGPL license files
run cp -f $TOOLCHAIN_LICENSES/COPYING $TOOLCHAIN_LICENSES/COPYING.LIB $TOOLCHAIN_PATH

# remove some unneeded files
run rm -f $TOOLCHAIN_PATH/bin/*-gccbug
run rm -rf $TOOLCHAIN_PATH/info
run rm -rf $TOOLCHAIN_PATH/man
run rm -rf $TOOLCHAIN_PATH/share
run rm -rf $TOOLCHAIN_PATH/lib/gcc/$ABI_CONFIGURE_TARGET/*/install-tools
run rm -rf $TOOLCHAIN_PATH/lib/gcc/$ABI_CONFIGURE_TARGET/*/plugin
run rm -rf $TOOLCHAIN_PATH/libexec/gcc/$ABI_CONFIGURE_TARGET/*/install-tools
run rm -rf $TOOLCHAIN_PATH/lib32/libiberty.a
run rm -rf $(find $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib -name libiberty.a -print)

# Remove libstdc++ for now (will add it differently later)
# We had to build it to get libsupc++ which we keep.
run rm -rf $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib/libstdc++.*
run rm -rf $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib/*/libstdc++.*
run rm -rf $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/include/c++

# Remove shared libgcc
run rm -rf $(find $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib -name 'libgcc_s.*' -print)

# Move libobjc files to the $GNUOBJC_SRCDIR
GNUOBJC_SRCDIR=$NDK_DIR/$GNUOBJC_SUBDIR
rm -rf $GNUOBJC_SRCDIR/include/$GCC_VERSION $GNUOBJC_SRCDIR/libs/$GCC_VERSION

# Move includes
copy_directory "$TOOLCHAIN_PATH/lib/gcc/$ABI_CONFIGURE_TARGET/$GCC_VERSION/include/objc" "$GNUOBJC_SRCDIR/include/$GCC_VERSION/objc"
run rm -rf $TOOLCHAIN_PATH/lib/gcc/$ABI_CONFIGURE_TARGET/$GCC_VERSION/include/objc

# Move libs
for f in $(find $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib -name libgnuobjc_shared.a -print); do
    run mv -f $f $(dirname $f)/libgnuobjc_static.a
done
case "$ARCH" in
    arm)
        for lib in libgnuobjc_shared.so libgnuobjc_static.a; do
            copy_file_list "$TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib/thumb" "$GNUOBJC_SRCDIR/libs/armeabi/$GCC_VERSION" "$lib"
            copy_file_list "$TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib/armv7-a" "$GNUOBJC_SRCDIR/libs/armeabi-v7a/$GCC_VERSION" "$lib"
        done
        ;;
    x86)
        for lib in libgnuobjc_shared.so libgnuobjc_static.a; do
            copy_file_list "$TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib" "$GNUOBJC_SRCDIR/libs/x86/$GCC_VERSION" "$lib"
        done
        ;;
    *)
        dump "ERROR: Unsupported NDK architecture!"
esac
run rm -rf $(find $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib -name 'libgnuobjc*' -print)
run rm -rf $(find $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/lib -name 'libobjc*' -print)

# strip binaries to reduce final package size
run strip $TOOLCHAIN_PATH/bin/*
run strip $TOOLCHAIN_PATH/$ABI_CONFIGURE_TARGET/bin/*
run strip $TOOLCHAIN_PATH/libexec/gcc/*/*/cc1$HOST_EXE
run strip $TOOLCHAIN_PATH/libexec/gcc/*/*/cc1plus$HOST_EXE
run strip $TOOLCHAIN_PATH/libexec/gcc/*/*/collect2$HOST_EXE

# copy SOURCES file if present
if [ -f "$SRC_DIR/SOURCES" ]; then
    cp "$SRC_DIR/SOURCES" "$TOOLCHAIN_PATH/SOURCES"
fi

if [ "$PACKAGE_DIR" ]; then
    ARCHIVE="$TOOLCHAIN-$HOST_TAG.tar.bz2"
    SUBDIR=$(get_toolchain_install_subdir $TOOLCHAIN $HOST_TAG)
    dump "Packaging $ARCHIVE"
    pack_archive "$PACKAGE_DIR/$ARCHIVE" "$NDK_DIR" "$SUBDIR"
    fail_panic "Could not package $ABI-$GCC_VERSION toolchain binaries"

    # Now, package objc files
    PACKAGE="$PACKAGE_DIR/gnu-libobjc-headers-$GCC_VERSION.tar.bz2"
    dump "Packaging: $PACKAGE"
    pack_archive "$PACKAGE" "$NDK_DIR" "$GNUOBJC_SUBDIR/include/$GCC_VERSION"
    fail_panic "Could not package $GCC_VERSION gnuobjc headers"

    case $ARCH in
        arm)
            ABIS="armeabi armeabi-v7a"
            ;;
        x86)
            ABIS="x86"
            ;;
        *)
            dump "ERROR: Unknown ABI: $ABI"
            exit 1
    esac
    for ABI in $ABIS; do
        FILES=""
        for LIB in libgnuobjc_static.a libgnuobjc_shared.so; do
            FILES="$FILES $GNUOBJC_SUBDIR/libs/$ABI/$GCC_VERSION/$LIB"
        done
        PACKAGE="$PACKAGE_DIR/gnu-libobjc-libs-$ABI-$GCC_VERSION.tar.bz2"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Could not package $ABI-$GCC_VERSION gnuobjc binaries"
    done
fi

dump "Done."
if [ -z "$OPTION_BUILD_OUT" ] ; then
    rm -rf $BUILD_OUT
fi
