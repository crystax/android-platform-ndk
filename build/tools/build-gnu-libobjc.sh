#!/bin/bash
#
# Copyright (C) 2011, 2012, 2013, 2014, 2015 The Android Open Source Project
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
#  This shell script is used to rebuild the prebuilt GNU libobjc binaries
#  from sources. It requires an NDK installation that contains valid plaforms
#  files and toolchain binaries.
#

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir>"

PROGRAM_DESCRIPTION=\
"Rebuild the prebuilt GNU libobjc binaries for the Android NDK.

This script is called when packaging a new NDK release. It will simply
rebuild the GNU libobjc static and shared libraries from
sources.

This requires a temporary NDK installation containing platforms and
toolchain binaries for all target architectures, as well as the path to
the corresponding gcc source tree.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$GNUOBJC_SUBDIR/<gcc-version>, but you can override this with the --out-dir=<path>
option.
"

GCC_VERSION_LIST="default"
register_var_option "--gcc-version-list=<vers>" GCC_VERSION_LIST "List of GCC versions"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>."

NDK_DIR=
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build."

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir."

ABIS=$(spaces_to_commas $PREBUILT_ABIS)
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs."

NO_MAKEFILE=
register_var_option "--no-makefile" NO_MAKEFILE "Do not use makefile to speed-up build"

register_jobs_option

register_try64_option

extract_parameters "$@"

if [ "$GCC_VERSION_LIST" = "default" ]; then
    GCC_VERSION_LIST=$DEFAULT_GCC_VERSION_LIST
fi

SRCDIR=$(echo $PARAMETERS | sed 1q)
check_toolchain_src_dir "$SRCDIR"

ABIS=$(commas_to_spaces $ABIS)
# since we do not build armeabi-v7a, armeabi-v7a-hard specific version,
# exclude it from the list
if [ $(echo $ABIS | tr ' ' '\n' | grep armeabi | wc -l) -gt 1 ]; then
    ABIS="armeabi "$(echo $ABIS | tr ' ' '\n' | grep -v armeabi | tr '\n' ' ')
fi

# Handle NDK_DIR
if [ -z "$NDK_DIR" ] ; then
    NDK_DIR=$ANDROID_NDK_ROOT
    log "Auto-config: --ndk-dir=$NDK_DIR"
else
    if [ ! -d "$NDK_DIR" ] ; then
        echo "ERROR: NDK directory does not exists: $NDK_DIR"
        exit 1
    fi
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-gnuobjc
else
    BUILD_DIR=$OPTION_BUILD_DIR
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

BUILD_SRCDIR=$SRCDIR/build

# $1: ABI name
# $2: Build directory
# $3: GCC version
build_gnuobjc_for_abi ()
{
    local ARCH SYSROOT
    local ABI=$1
    local BUILDDIR="$2"
    local GCC_VERSION="$3"
    local SRC OBJ OBJECTS CFLAGS OLD_ABI PROJECT

    prepare_target_build $ABI $PLATFORM $NDK_DIR
    fail_panic "Could not setup target build."

    BUILDDIR=$BUILDDIR/$ABI-$GCC_VERSION
    INSTALLDIR=$BUILDDIR/install

    run mkdir -p $BUILDDIR

    ARCH=$(convert_abi_to_arch $ABI)

    OLD_ABI=$ABI
    TOOLCHAIN=$(get_toolchain_name_for_arch $ARCH $GCC_VERSION)
    ABI_CONFIGURE_EXTRA_FLAGS=
    parse_toolchain_name $TOOLCHAIN
    ABI=$OLD_ABI

    set_toolchain_ndk $NDK_DIR $TOOLCHAIN

    SRC_SYSROOT=$NDK_DIR/$(get_default_platform_sysroot_for_arch $ARCH)
    SYSROOT=$INSTALLDIR/sysroot
    dump "Sysroot  : Copying: $SRC_SYSROOT --> $SYSROOT"
    mkdir -p $SYSROOT && (cd $SRC_SYSROOT && tar ch *) | (cd $SYSROOT && tar x)
    fail_panic "Error while copying sysroot files. See $TMPLOG"

    # Sanity check
    if [ ! -f "$SYSROOT/usr/lib/libc.a" ]; then
        echo "ERROR: Empty sysroot! you probably need to run gen-platforms.sh before this script."
        exit 1
    fi
    if [ ! -f "$SYSROOT/usr/lib/libc.so" ]; then
        echo "ERROR: Sysroot misses shared libraries! you probably need to run gen-platforms.sh"
        echo "*without* the --minimal flag before running this script."
        exit 1
    fi

    CFLAGS_FOR_TARGET=$ABI_CFLAGS_FOR_TARGET
    CXXFLAGS_FOR_TARGET=$ABI_CXXFLAGS_FOR_TARGET
    LDFLAGS_FOR_TARGET=$ABI_LDFLAGS_FOR_TARGET" -Wl,-Bdynamic,-lcrystax"

    PROJECT="gnuobjc gcc-$GCC_VERSION $ABI"

    export CC CXX
    export CFLAGS_FOR_TARGET CXXFLAGS_FOR_TARGET LDFLAGS_FOR_TARGET
    # Needed to build a 32-bit gmp on 64-bit systems
    export ABI=$HOST_GMP_ABI
    export CFLAGS="$HOST_CFLAGS"
    export CXXFLAGS="$HOST_CFLAGS"
    export LDFLAGS="$HOST_LDFLAGS"

    echo "$PROJECT: configuring"
    cd $BUILDDIR &&
    run $BUILD_SRCDIR/configure \
        --target=$ABI_CONFIGURE_TARGET \
        --enable-initfini-array \
        --host=$ABI_CONFIGURE_HOST \
        --build=$ABI_CONFIGURE_BUILD \
        --disable-nls \
        --prefix=$INSTALLDIR \
        --with-sysroot=$SYSROOT \
        --with-gcc-version=$GCC_VERSION \
        --with-binutils-version=$(get_default_binutils_version_for_gcc $TOOLCHAIN) \
        --with-gdb-version=$(get_default_gdb_version_for_gcc $TOOLCHAIN) \
        --with-mpfr-version=$DEFAULT_MPFR_VERSION \
        --with-mpc-version=$DEFAULT_MPC_VERSION \
        --with-gmp-version=$DEFAULT_GMP_VERSION \
        --with-expat-version=$DEFAULT_EXPAT_VERSION \
        --disable-bootstrap \
        --disable-libquadmath \
        --disable-plugin \
        --with-gxx-include-dir=$INSTALLDIR/include/c++/$GCC_VERSION \
        $ABI_CONFIGURE_EXTRA_FLAGS
    fail_panic "Could not configure $PROJECT"

    echo "$PROJECT: building"
    cd $BUILDDIR &&
    run make -j$NUM_JOBS build-target-libobjc
    fail_panic "Could not build $PROJECT"

    echo "$PROJECT: installing"
    cd $BUILDDIR &&
    run make install-target-libobjc
    fail_panic "Could not install $PROJECT"

    # Now, prepare final static/shared libraries
    echo "$PROJECT: preparing static/shared binaries"
    local LDIR=lib
    if [ "$ARCH" != "${ARCH%%64*}" ]; then
        # todo zuav: Can't call $(get_default_libdir_for_arch $ARCH) which contain hack for arm64 and mips64
        LDIR=lib64
    fi
    local LIBDIR=$INSTALLDIR/$ABI_CONFIGURE_TARGET/$LDIR
    for dir in $LIBDIR $LIBDIR/armv7-a $LIBDIR/armv7-a/hard; do
        [ -d $dir ] || continue
        
        local hardopts=
        local hardoptsld=
        if [ "$dir" = "$LIBDIR/armv7-a/hard" ]; then
            hardopts="-mhard-float -mfloat-abi=hard"
            hardoptsld=",-lm_hard,--no-warn-mismatch"
        fi

        run mv $dir/libobjc.a $dir/libgnuobjc_static.a

        run mkdir -p $BUILDDIR/shared &&
        run cd $BUILDDIR/shared &&
        run $TOOLCHAIN_PREFIX-ar x $dir/libgnuobjc_static.a &&
        run $TOOLCHAIN_PREFIX-gcc $hardopts -o $dir/libgnuobjc_shared.so \
            -shared -Wl,-soname,libgnuobjc_shared.so$hardoptsld --sysroot=$SYSROOT *.o
        fail_panic "Could not prepare final static/shared binaries for $PROJECT"
        run rm -rf $BUILDDIR/shared
    done
}

HAS_COMMON_HEADERS=

# $1: ABI
# $2: Build directory
# $3: GCC_VERSION
copy_gnuobjc_libs ()
{
    local ABI="$1"
    local BUILDDIR="$2"
    local ARCH=$(convert_abi_to_arch $ABI)
    local GCC_VERSION="$3"
    local PREFIX=$(get_default_toolchain_prefix_for_arch $ARCH)
    PREFIX=${PREFIX%%-}

    local SDIR="$BUILDDIR/$ABI-$GCC_VERSION/install"
    local DDIR="$NDK_DIR/$GNUOBJC_SUBDIR/$GCC_VERSION"

    local GCC_VERSION_NO_DOT=$(echo $GCC_VERSION|sed 's/\./_/g')
    # Copy the common headers only once per gcc version
    if [ -z `var_value HAS_COMMON_HEADERS_$GCC_VERSION_NO_DOT` ]; then
        local GCC_BASE_VERSION=`cat $SRCDIR/gcc/gcc-$GCC_VERSION/gcc/BASE-VER`
        copy_directory "$SDIR/lib/gcc/$PREFIX/$GCC_BASE_VERSION/include" "$DDIR/include"
        eval HAS_COMMON_HEADERS_$GCC_VERSION_NO_DOT=true
    fi

    rm -rf "$DDIR/libs/$ABI"

    local LDIR=lib
    if [ "$ARCH" != "${ARCH%%64*}" ]; then
        # todo zuav: Can't call $(get_default_libdir_for_arch $ARCH) which contain hack for arm64 and mips64
        LDIR=lib64
    fi
    # Copy the ABI-specific libraries
    copy_file_list "$SDIR/$PREFIX/$LDIR" "$DDIR/libs/$ABI" libgnuobjc_static.a libgnuobjc_shared.so
    if [ -d $SDIR/$PREFIX/$LDIR/armv7-a ]; then
        copy_file_list "$SDIR/$PREFIX/$LDIR/armv7-a"      "$DDIR/libs/armeabi-v7a"      libgnuobjc_static.a libgnuobjc_shared.so
        copy_file_list "$SDIR/$PREFIX/$LDIR/armv7-a/hard" "$DDIR/libs/armeabi-v7a-hard" libgnuobjc_static.a libgnuobjc_shared.so
    fi
}

GCC_VERSION_LIST=$(commas_to_spaces $GCC_VERSION_LIST)
dump "Building for abis: $ABIS; gcc versions: $GCC_VERSION_LIST"
BUILT_GCC_VERSION_LIST=""
BUILT_ABIS=""
for VERSION in $GCC_VERSION_LIST; do
    PACKAGE_NAME="gnu-libobjc-headers-$VERSION.tar.bz2"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    for ABI in $ABIS; do
        if [ "$ABI" != "${ABI%%64*}" -a "$VERSION" != "4.9" ]; then
            dump "Skipping building $ABI and $VERSION"
        else
            DO_BUILD_PACKAGE="yes"
            if [ -n "$PACKAGE_DIR" ]; then
                PACKAGE_NAME="gnu-libobjc-libs-$VERSION-$ABI.tar.bz2"
                echo "Look for: $PACKAGE_NAME"
                try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
                if [ $? = 0 ]; then
                    DO_BUILD_PACKAGE="no"
                    if [ "$ABI" == "armeabi" ]; then
                        # todo zuav: check for absent packages and
                        # rebuild all armeabi if v7a or v7a-hard packages not found
                        try_cached_package "$PACKAGE_DIR" "gnu-libobjc-libs-$VERSION-armeabi-v7a.tar.bz2" no_exit
                        try_cached_package "$PACKAGE_DIR" "gnu-libobjc-libs-$VERSION-armeabi-v7a-hard.tar.bz2" no_exit
                    fi
                else
                    BUILT_GCC_VERSION_LIST="$BUILT_GCC_VERSION_LIST $VERSION"
                    BUILT_ABIS="$BUILT_ABIS $ABI"
                    if [ "$ABI" != "${ABI%%armeabi*}" ]; then
                        BUILT_ABIS="$BUILT_ABIS armeabi-v7a armeabi-v7a-hard"
                    fi
                fi
            fi
            if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
                build_gnuobjc_for_abi $ABI "$BUILD_DIR" $VERSION
                copy_gnuobjc_libs $ABI "$BUILD_DIR" $VERSION
            fi
        fi
    done
done

# If needed, package files into tarballs
if [ -n "$PACKAGE_DIR" ] ; then
    for VERSION in $BUILT_GCC_VERSION_LIST; do
        # First, the headers as a single package for a given gcc version
        PACKAGE_NAME="gnu-libobjc-headers-$VERSION.tar.bz2"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$GNUOBJC_SUBDIR/$VERSION/include"
        fail_panic "Could not package $VERSION GNU libobjc headers!"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"

        # Then, one package per version/ABI for libraries
        PACKAGE_NAME=""
        for ABI in $BUILT_ABIS; do
            if [ "$ABI" != "${ABI%%64*}" -a "$VERSION" != "4.9" ]; then
                dump "Skipping packaging $ABI and $VERSION"
            else
                FILES=""
                for LIB in libgnuobjc_static.a libgnuobjc_shared.so; do
                    FILES="$FILES $GNUOBJC_SUBDIR/$VERSION/libs/$ABI/$LIB"
                done
                PACKAGE_NAME="gnu-libobjc-libs-$VERSION-$ABI.tar.bz2"
                PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
                dump "Packaging: $PACKAGE"
                pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
                fail_panic "Could not package $ABI GNU libobjc binaries!"
                cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
            fi
        done
    done
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    log "Cleaning up..."
    rm -rf $BUILD_DIR
else
    log "Don't forget to cleanup: $BUILD_DIR"
fi

log "Done!"
