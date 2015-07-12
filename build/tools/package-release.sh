#!/bin/bash
#
# Copyright (C) 2009-2010, 2014, 2015 The Android Open Source Project
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
# This script is used to package complete Android NDK release packages.
#
# You will need prebuilt toolchain binary tarballs or a previous
# NDK release package to do that.
#
# See make-release.sh if you want to make a new release completely from
# scratch.
#

. `dirname $0`/prebuilt-common.sh

NDK_ROOT_DIR="$ANDROID_NDK_ROOT"

# the list of platforms / API levels we want to package in
# this release. This can be overriden with the --platforms
# option, see below.
#
PLATFORMS="$API_LEVELS"

# the default release name (use today's date)
RELEASE=`date +%Y%m%d`
register_var_option "--release=<name>" RELEASE "Specify release name"

# the directory containing all prebuilts
PREBUILT_DIR=
register_var_option "--prebuilt-dir=<path>" PREBUILT_DIR "Specify prebuilt directory"

# a prebuilt NDK archive (.zip file). empty means don't use any
PREBUILT_NDK=
register_var_option "--prebuilt-ndk=<file>" PREBUILT_NDK "Specify prebuilt ndk package"

# the list of supported host development systems
SYSTEMS=$DEFAULT_SYSTEMS
register_var_option "--systems=<list>" SYSTEMS "Specify host systems"

# ARCH to build for
ARCHS="$DEFAULT_ARCHS"
register_var_option "--arch=<arch>" ARCHS "Specify target architecture(s)"

# set to 'yes' if we should use 'git ls-files' to list the files to
# be copied into the archive.
NO_GIT=no
register_var_option "--no-git" NO_GIT "Don't use git to list input files, take all of them."

# set of toolchain prebuilts we need to package
OPTION_TOOLCHAINS=
register_var_option "--toolchains=<list>" OPTION_TOOLCHAINS "Specify list of toolchains."

# set of platforms to package (all by default)
register_var_option "--platforms=<list>" PLATFORMS "Specify API levels"

# the package prefix
PREFIX=android-ndk
register_var_option "--prefix=<name>" PREFIX "Specify package prefix"

# default location for generated packages
OUT_DIR=$TMPDIR/release
OPTION_OUT_DIR=
register_var_option "--out-dir=<path>" OPTION_OUT_DIR "Specify output package directory" "$OUT_DIR"

# Find the location of the platforms root directory
DEVELOPMENT_ROOT=`dirname $ANDROID_NDK_ROOT`/development/ndk
register_var_option "--development-root=<path>" DEVELOPMENT_ROOT "Specify platforms/samples directory"

GCC_VERSION_LIST="default" # it's arch defined by default so use default keyword
register_var_option "--gcc-version-list=<vers>" GCC_VERSION_LIST "List of GCC release versions"

LLVM_VERSION_LIST=$DEFAULT_LLVM_VERSION_LIST
register_var_option "--llvm-version-list=<versions>" LLVM_VERSION_LIST "List of LLVM release versions"

register_try64_option

SEPARATE_64=no
register_option "--separate-64" do_SEPARATE_64 "Separate 64-bit host toolchain to its own package"
do_SEPARATE_64 ()
{
    if [ "$TRY64" = "yes" ]; then
        echo "ERROR: You cannot use both --try-64 and --separate-64 at the same time."
        exit 1
    fi
    SEPARATE_64=yes;
}

PROGRAM_PARAMETERS=
PROGRAM_DESCRIPTION=\
"Package a new set of release packages for the Android NDK.

You will need to have generated one or more prebuilt binary tarballs
with the build/tools/rebuild-all-prebuilts.sh script. These files should
be named like <toolname>-<system>.tar.bz2, where <toolname> is an arbitrary
tool name, and <system> is one of: $SYSTEMS

Use the --prebuilt-dir=<path> option to build release packages from the
binary tarballs stored in <path>.

Alternatively, you can use --prebuilt-ndk=<file> where <file> is the path
to a previous NDK release package. It will be used to extract the toolchain
binaries and copy them to your new release. Only use this for experimental
package releases.

The generated release packages will be stored in a temporary directory that
will be printed at the end of the generation process.
"

extract_parameters "$@"

# Ensure that SYSTEMS is space-separated
SYSTEMS=$(commas_to_spaces $SYSTEMS)

ARCHS=$(commas_to_spaces $ARCHS)

# Compute ABIS from ARCHS
ABIS=
for ARCH in $ARCHS; do
    DEFAULT_ABIS=$(get_default_abis_for_arch $ARCH)
    if [ -z "$ABIS" ]; then
        ABIS=$DEFAULT_ABIS
    else
        ABIS=$ABIS" $DEFAULT_ABIS"
    fi
done

# Convert comma-separated list to space-separated list
LLVM_VERSION_LIST=$(commas_to_spaces $LLVM_VERSION_LIST)

# If --arch is used to list x86 as a target architecture, Add x86-4.8 to
# the list of default toolchains to package. That is, unless you also
# explicitely use --toolchains=<list>
#
# Ensure that TOOLCHAINS is space-separated after this.
#
if [ "$OPTION_TOOLCHAINS" != "$TOOLCHAINS" ]; then
    TOOLCHAINS=$(commas_to_spaces $OPTION_TOOLCHAINS)
else
    for ARCH in $ARCHS; do
       case $ARCH in
           arm|arm64|x86|x86_64|mips|mips64) TOOLCHAINS=$TOOLCHAINS" "$(get_toolchain_name_list_for_arch $ARCH) ;;
           *) echo "ERROR: Unknown arch to package: $ARCH"; exit 1 ;;
       esac
    done
    TOOLCHAINS=$(commas_to_spaces $TOOLCHAINS)
fi

if [ "$GCC_VERSION_LIST" != "default" ]; then
   TOOLCHAIN_NAMES=
   for VERSION in $(commas_to_spaces $GCC_VERSION_LIST); do
      for TOOLCHAIN in $TOOLCHAINS; do
         if [ $TOOLCHAIN != ${TOOLCHAIN%%$VERSION} ]; then
            TOOLCHAIN_NAMES="$TOOLCHAIN $TOOLCHAIN_NAMES"
         fi
      done
   done
   TOOLCHAINS=$TOOLCHAIN_NAMES
fi

# Check the prebuilt path
#
if [ -n "$PREBUILT_NDK" -a -n "$PREBUILT_DIR" ] ; then
    echo "ERROR: You cannot use both --prebuilt-ndk and --prebuilt-dir at the same time."
    exit 1
fi

if [ -z "$PREBUILT_DIR" -a -z "$PREBUILT_NDK" ] ; then
    echo "ERROR: You must use one of --prebuilt-dir or --prebuilt-ndk. See --help for details."
    exit 1
fi

# Check the option directory.
if [ -n "$OPTION_OUT_DIR" ] ; then
    OUT_DIR="$OPTION_OUT_DIR"
    mkdir -p $OUT_DIR
    if [ $? != 0 ] ; then
        echo "ERROR: Could not create output directory: $OUT_DIR"
        exit 1
    fi
else
    rm -rf $OUT_DIR && mkdir -p $OUT_DIR
fi

# Handle the prebuilt binaries now
#
if [ -n "$PREBUILT_DIR" ] ; then
    if [ ! -d "$PREBUILT_DIR" ] ; then
        echo "ERROR: the --prebuilt-dir argument is not a directory: $PREBUILT_DIR"
        exit 1
    fi
    if [ -z "$SYSTEMS" ] ; then
        echo "ERROR: Your systems list is empty, use --systems=LIST to specify a different one."
        exit 1
    fi
else
    if [ ! -f "$PREBUILT_NDK" ] ; then
        echo "ERROR: the --prebuilt-ndk argument is not a file: $PREBUILT_NDK"
        exit 1
    fi
    # Check that the name ends with the proper host tag
    HOST_NDK_SUFFIX="$HOST_TAG.zip"
    echo "$PREBUILT_NDK" | grep -q "$HOST_NDK_SUFFIX"
    fail_panic "The name of the prebuilt NDK must end in $HOST_NDK_SUFFIX"
    SYSTEMS=$HOST_TAG
fi

echo "Architectures: $ARCHS"
echo "CPU ABIs: $ABIS"
echo "GCC Toolchains: $TOOLCHAINS"
echo "LLVM Toolchains: $LLVM_VERSION_LIST"
echo "Host systems: $SYSTEMS"


# The list of git files to copy into the archives
if [ "$NO_GIT" != "yes" ] ; then
    echo "Collecting sources from git (use --no-git to copy all files instead)."
    GIT_FILES=`cd $NDK_ROOT_DIR && git ls-files`
else
    echo "Collecting all sources files under tree."
    # Cleanup everything that is likely to not be part of the final NDK
    # i.e. generated files...
    rm -rf $NDK_ROOT_DIR/samples/*/obj
    rm -rf $NDK_ROOT_DIR/samples/*/libs
    # Get all files under the NDK root
    GIT_FILES=`cd $NDK_ROOT_DIR && find .`
    GIT_FILES=`echo $GIT_FILES | sed -e "s!\./!!g"`
fi

# temporary directory used for packaging
TMPDIR=$NDK_TMPDIR

RELEASE_PREFIX=$PREFIX-$RELEASE

# ensure that the generated files are ug+rx
umask 0022

# Translate name to 64-bit's counterpart
# $1: prebuilt name
name64 ()
{
    local NAME=$1
    case $NAME in
        *windows)
            NAME=${NAME}-x86_64
            ;;
        *linux-x86|*darwin-x86|*windows-x86)
            NAME=${NAME}_64
            ;;
    esac
    echo $NAME
}

# Unpack a prebuilt into specified destination directory
# $1: prebuilt name, relative to $PREBUILT_DIR
# $2: destination directory
# $3: optional destination directory for 64-bit toolchain
# $4: optional flag to use 32-bit prebuilt in place of 64-bit
unpack_prebuilt ()
{
    local PREBUILT=
    local PREBUILT64=null
    local DDIR="$2"
    local DDIR64="${3:-$DDIR}"
    local USE32="${4:-no}"

    if [ "$TRY64" = "yes" -a "$USE32" = "no" ]; then
        PREBUILT=`name64 $1`
    else
        PREBUILT=$1
        PREBUILT64=`name64 $1`
    fi

    PREBUILT=${PREBUILT}.tar.bz2
    PREBUILT64=${PREBUILT64}.tar.bz2

    echo "Unpacking $PREBUILT"
    if [ -f "$PREBUILT_DIR/$PREBUILT" ] ; then
        unpack_archive "$PREBUILT_DIR/$PREBUILT" "$DDIR"
        fail_panic "Could not unpack prebuilt $PREBUILT. Aborting."
        if [ -f "$PREBUILT_DIR/$PREBUILT64" ] ; then
            echo "Unpacking $PREBUILT64"
            unpack_archive "$PREBUILT_DIR/$PREBUILT64" "$DDIR64"
            fail_panic "Could not unpack prebuilt $PREBUILT64. Aborting."
        fi
    else
        echo "WARNING: Could not find $PREBUILT in $PREBUILT_DIR"
    fi
}

# Copy a prebuilt directory from the previous
# $1: Source directory relative to
copy_prebuilt ()
{
    local SUBDIR="$1"
    if [ -d "$1" ] ; then
        echo "Copying: $SUBDIR"
        copy_directory "$SUBDIR" "$DSTDIR/$2"
    else
        echo "Copying: $1"
        copy_file_list "$(dirname $1)" "$DSTDIR/$2" "$(basename $1)"
    fi
}

# Pack a release
#
# $1: archive file path (including extension)
# $2: source directory for archive content
# $3: directory to pack
pack_release ()
{
    local archive="$1"
    local srcdir="$2"
    local reldir="$3"
    local ext="${archive##*.}"

    local win7z=$NDK_ROOT_DIR/../prebuilts/7zip/windows/64/7z.exe

    local flags="a -t7z  -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on"

    if [ "`basename $archive`" = "$archive" ] ; then
        archive="`pwd`/$archive"
    fi

    local pack_cmd
    if [ "$ext" = "exe" ] ; then
        if [ -f $win7z ]; then
            pack_cmd="wine $win7z $flags -sfx"
        else
            pack_cmd="7z $flags"
            archive=${archive%%.$ext}.7z
        fi
    else
        pack_cmd="7z $flags -sfx"
    fi

    mkdir -p `dirname $archive`
    (
        cd $srcdir || exit 1
        $pack_cmd "$archive" "$reldir" | {
            cnt=0
            total=0
            while read line; do
                let cnt+=1
                test $cnt -ge 5000 || continue
                let total+=$cnt
                cnt=0
                echo "Packed $total files"
            done
            let total+=$cnt
            test $cnt -eq 0 || echo "Packed $total files"
        }
        test ${PIPESTATUS[0]} -eq 0 || exit 1
        chmod a+x $archive || exit 1
    )
    fail_panic "Can't pack $archive"
}

rm -rf $TMPDIR && mkdir -p $TMPDIR

# Unpack the previous NDK package if any
if [ -n "$PREBUILT_NDK" ] ; then
    echo "Unpacking prebuilt toolchains from $PREBUILT_NDK"
    UNZIP_DIR=$TMPDIR/prev-ndk
    rm -rf $UNZIP_DIR && mkdir -p $UNZIP_DIR
    fail_panic "Could not create temporary directory: $UNZIP_DIR"
    unpack_archive "$PREBUILT_NDK" "$UNZIP_DIR"
    fail_panic "Could not unzip NDK package $PREBUILT_NDK"
fi

# first create the reference ndk directory from the git reference
echo "Creating reference from source files"
REFERENCE=$TMPDIR/reference && rm -rf $REFERENCE/* &&
copy_file_list "$NDK_ROOT_DIR" "$REFERENCE" $GIT_FILES &&
rm -f $REFERENCE/Android.mk
fail_panic "Could not create reference. Aborting."

echo "Coping LIBC++ headers and sources"
# first remove symlink
rm "$REFERENCE/sources/cxx-stl/llvm-libc++/libcxx"
fail_panic "Could not remove LIBCXX link. Aborting."
# create dir and copy headers and sources
mkdir -p "$REFERENCE/sources/cxx-stl/llvm-libc++/libcxx"
cp -r "$NDK_ROOT_DIR/sources/cxx-stl/llvm-libc++/libcxx/include" "$REFERENCE/sources/cxx-stl/llvm-libc++/libcxx/"
fail_panic "Could not copy LIBCXX include files. Aborting."
cp -r "$NDK_ROOT_DIR/sources/cxx-stl/llvm-libc++/libcxx/src" "$REFERENCE/sources/cxx-stl/llvm-libc++/libcxx/"
fail_panic "Could not copy LIBCXX src files. Aborting."
cp -r "$NDK_ROOT_DIR/sources/cxx-stl/llvm-libc++/libcxx/test" "$REFERENCE/sources/cxx-stl/llvm-libc++/libcxx/"
fail_panic "Could not copy LIBCXX test files. Aborting."

echo "Copying OpenPTS sources"
OPENPTS_SUBDIR="sources/crystax/tests/openpts"
OPENPTS_SRCDIR="$NDK_ROOT_DIR/$OPENPTS_SUBDIR"
OPENPTS_DSTDIR="$REFERENCE/$OPENPTS_SUBDIR"
rm -Rf "$OPENPTS_DSTDIR" && mkdir -p "$OPENPTS_DSTDIR" && rsync -aL --exclude=/.git "$OPENPTS_SRCDIR/" "$OPENPTS_DSTDIR/"
fail_panic "Could not copy OpenPTS sources"

# Copy platform and sample files
if [ "$PREBUILT_DIR" ]; then
    echo "Unpacking platform files" &&
    unpack_archive "$PREBUILT_DIR/platforms.tar.bz2" "$REFERENCE" &&
    echo "Unpacking samples files" &&
    unpack_archive "$PREBUILT_DIR/samples.tar.bz2" "$REFERENCE"
    fail_panic "Could not unpack platform and sample files"
elif [ "$PREBUILT_NDK" ]; then
    echo "ERROR: NOT IMPLEMENTED!"
    exit 1
else
    # copy platform and sample files
    echo "Copying platform and sample files"
    FLAGS="--src-dir=$DEVELOPMENT_ROOT --dst-dir=$REFERENCE"
    if [ "$VERBOSE2" = "yes" ] ; then
        FLAGS="$FLAGS --verbose"
    fi

    FLAGS="$FLAGS --platform=$(spaces_to_commas $PLATFORMS)"
    FLAGS="$FLAGS --arch=$(spaces_to_commas $ARCHS)"
    $NDK_ROOT_DIR/build/tools/gen-platforms.sh $FLAGS
    fail_panic "Could not copy platform files. Aborting."
fi

# Remove the source for host tools to make the final package smaller
rm -rf $REFERENCE/sources/host-tools

# Remove leftovers, just in case...
rm -rf $REFERENCE/samples/*/{obj,libs,build.xml,local.properties,Android.mk} &&
rm -rf $REFERENCE/tests/build/*/{obj,libs} &&
rm -rf $REFERENCE/tests/device/*/{obj,libs}

# copy sources files
if [ -d $DEVELOPMENT_ROOT/sources ] ; then
    echo "Copying NDK sources files"
    copy_file_list "$DEVELOPMENT_ROOT" "$REFERENCE" "sources"
    fail_panic "Could not copy sources. Aborting."
fi

# Unpack prebuilt Objective-C, C++ runtimes headers and libraries
if [ -z "$PREBUILT_NDK" ]; then
    # Unpack gdbserver
    for ARCH in $ARCHS; do
        unpack_prebuilt $ARCH-gdbserver "$REFERENCE"
    done
    # Unpack Objective-C, C++ runtimes
    for VERSION in $DEFAULT_GCC_VERSION_LIST; do
        unpack_prebuilt gnu-libstdc++-headers-$VERSION "$REFERENCE"
        unpack_prebuilt gnu-libobjc-headers-$VERSION "$REFERENCE"
    done
    unpack_prebuilt gnustep-objc2-headers "$REFERENCE"

    unpack_prebuilt sqlite3-build-files "$REFERENCE"
    unpack_prebuilt sqlite3-headers "$REFERENCE"
    for VERSION in $LIBPNG_VERSIONS; do
        unpack_prebuilt libpng-$VERSION-headers "$REFERENCE"
    done
    for VERSION in $LIBJPEG_VERSIONS; do
        unpack_prebuilt libjpeg-$VERSION-headers "$REFERENCE"
    done
    for VERSION in $LIBJPEGTURBO_VERSIONS; do
        unpack_prebuilt libjpeg-turbo-$VERSION-headers "$REFERENCE"
    done
    for VERSION in $LIBTIFF_VERSIONS; do
        unpack_prebuilt libtiff-$VERSION-headers "$REFERENCE"
    done
    for VERSION in $ICU_VERSIONS; do
        unpack_prebuilt icu-$VERSION-build-files "$REFERENCE"
        unpack_prebuilt icu-$VERSION-headers "$REFERENCE"
    done
    for VERSION in $BOOST_VERSIONS; do
        unpack_prebuilt boost-$VERSION-build-files "$REFERENCE"
        unpack_prebuilt boost+icu-$VERSION-build-files "$REFERENCE"
        unpack_prebuilt boost-$VERSION-headers "$REFERENCE"
        unpack_prebuilt boost+icu-$VERSION-headers "$REFERENCE"
    done
    for ABI in $ABIS; do
        unpack_prebuilt crystax-libs-$ABI "$REFERENCE"
        unpack_prebuilt gabixx-libs-$ABI-g "$REFERENCE"
        unpack_prebuilt stlport-libs-$ABI-g "$REFERENCE"
        unpack_prebuilt libcxx-libs-$ABI-g "$REFERENCE"
        for VERSION in $DEFAULT_GCC_VERSION_LIST; do
            if [ "${ABI/64/}" != "$ABI" -a "$VERSION" = "4.8" ]; then
                # only gcc 4.9 is used for 64-bit abis
                continue
            fi
            unpack_prebuilt gnu-libstdc++-libs-$VERSION-$ABI-g "$REFERENCE"
            unpack_prebuilt gnu-libobjc-libs-$VERSION-$ABI "$REFERENCE"
        done
        unpack_prebuilt gnustep-objc2-libs-$ABI "$REFERENCE"
        unpack_prebuilt cocotron-$ABI "$REFERENCE"
        unpack_prebuilt sqlite3-libs-$ABI "$REFERENCE"
        for VERSION in $LIBPNG_VERSIONS; do
            unpack_prebuilt libpng-$VERSION-libs-$ABI "$REFERENCE"
        done
        for VERSION in $LIBJPEG_VERSIONS; do
            unpack_prebuilt libjpeg-$VERSION-libs-$ABI "$REFERENCE"
        done
        for VERSION in $LIBJPEGTURBO_VERSIONS; do
            unpack_prebuilt libjpeg-turbo-$VERSION-libs-$ABI "$REFERENCE"
        done
        for VERSION in $LIBTIFF_VERSIONS; do
            unpack_prebuilt libtiff-$VERSION-libs-$ABI "$REFERENCE"
        done
        for VERSION in $ICU_VERSIONS; do
            unpack_prebuilt icu-$VERSION-libs-$ABI "$REFERENCE"
        done
        for VERSION in $BOOST_VERSIONS; do
            unpack_prebuilt boost-$VERSION-libs-$ABI "$REFERENCE"
            unpack_prebuilt boost+icu-$VERSION-libs-$ABI "$REFERENCE"
        done
        unpack_prebuilt compiler-rt-libs-$ABI "$REFERENCE"
        unpack_prebuilt libgccunwind-libs-$ABI "$REFERENCE"
    done
fi

RELEASE_VERSION=${RELEASE_PREFIX%%-b*}
# create a release file named 'RELEASE.TXT' containing the release
# name. This is used by the build script to detect whether you're
# invoking the NDK from a release package or from the development
# tree.
#
if [ "$TRY64" = "yes" ]; then
    echo "$RELEASE_VERSION (64-bit)" > $REFERENCE/RELEASE.TXT
else
    echo "$RELEASE_VERSION" > $REFERENCE/RELEASE.TXT
fi

# Patch sysroot
$REFERENCE/sources/crystax/bin/patch-sysroot --libraries
fail_panic "Could not patch sysroot"

# Remove auto-generated libcrystax.* stubs from platforms
find $REFERENCE/platforms -name 'libcrystax.*' -delete

# Remove un-needed files
rm -f $REFERENCE/CleanSpec.mk
rm -Rf $REFERENCE/sources/crystax/vendor

# remove crew files
# later appropriate version of the crew will be cloned
rm -rf $REFERENCE/tools/crew

# now, for each system, create a package
#
DSTDIR=$TMPDIR/$RELEASE_VERSION
DSTDIR64=${DSTDIR}
if [ "$SEPARATE_64" = "yes" ] ; then
    DSTDIR64=$TMPDIR/64/${RELEASE_VERSION}
fi

SCRIPTS_DIR=$(dirname $(dirname $0))/scripts

for SYSTEM in $SYSTEMS; do
    echo "Preparing package for system $SYSTEM."
    BIN_RELEASE=$RELEASE_PREFIX-$SYSTEM
    rm -rf "$DSTDIR" "$DSTDIR64" &&
    mkdir -p "$DSTDIR" "$DSTDIR64" &&
    copy_directory "$REFERENCE" "$DSTDIR"
    fail_panic "Could not copy reference. Aborting."

    if [ "$DSTDIR" != "$DSTDIR64" ]; then
        copy_directory "$DSTDIR" "$DSTDIR64"
        echo "$RELEASE_VERSION (64-bit)" > $DSTDIR64/RELEASE.TXT
    fi

    if [ "$PREBUILT_NDK" ]; then
        cd $UNZIP_DIR/android-ndk-* && cp -rP toolchains/* $DSTDIR/toolchains/
        fail_panic "Could not copy toolchain files from $PREBUILT_NDK"

        if [ -d "$DSTDIR/$CRYSTAX_SUBDIR" ]; then
            CRYSTAX_ABIS=$PREBUILT_ABIS $UNKNOWN_ABIS
            for CRYSTAX_ABI in $CRYSTAX_ABIS; do
                copy_prebuilt "$CRYSTAX_SUBDIR/libs/$CRYSTAX_ABI" "$CRYSTAX_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find CrystaX source tree!"
        fi

        if [ -d "$DSTDIR/$GABIXX_SUBDIR" ]; then
            GABIXX_ABIS=$PREBUILT_ABIS
            for GABIXX_ABI in $GABIXX_ABIS; do
                copy_prebuilt "$GABIXX_SUBDIR/libs/$GABIXX_ABI" "$GABIXX_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find GAbi++ source tree!"
        fi

        if [ -d "$DSTDIR/$STLPORT_SUBDIR" ] ; then
            STLPORT_ABIS=$PREBUILT_ABIS
            for STL_ABI in $STLPORT_ABIS; do
                copy_prebuilt "$STLPORT_SUBDIR/libs/$STL_ABI" "$STLPORT_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find STLport source tree!"
        fi

        if [ -d "$DSTDIR/$LIBCXX_SUBDIR" ]; then
            LIBCXX_ABIS=$PREBUILT_ABIS
            for STL_ABI in $LIBCXX_ABIS; do
                copy_prebuilt "$LIBCXX_SUBDIR/libs/$STL_ABI" "$LIBCXX_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find Libc++ source tree!"
        fi

        for VERSION in $DEFAULT_GCC_VERSION_LIST; do
            copy_prebuilt "$GNUSTL_SUBDIR/$VERSION/include" "$GNUSTL_SUBDIR/$VERSION/"
            for STL_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$GNUSTL_SUBDIR/$VERSION/libs/$STL_ABI" "$GNUSTL_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $DEFAULT_GCC_VERSION_LIST; do
            copy_prebuilt "$GNUOBJC_SUBDIR/$VERSION/include" "$GNUOBJC_SUBDIR/$VERSION/"
            for OBJC_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$GNUOBJC_SUBDIR/$VERSION/libs/$OBJC_ABI" "$GNUOBJC_SUBDIR/$VERSION/libs"
            done
        done

        copy_prebuilt "$GNUSTEP_OBJC2_SUBDIR/include" "$GNUSTEP_OBJC2_SUBDIR/"
        for OBJC2_ABI in $PREBUILT_ABIS; do
            copy_prebuilt "$GNUSTEP_OBJC2_SUBDIR/libs/$OBJC2_ABI" "$GNUSTEP_OBJC2_SUBDIR/libs"
        done

        for COCOTRON_ABI in $PREBUILT_ABIS; do
            copy_prebuilt "$COCOTRON_SUBDIR/frameworks/$COCOTRON_ABI" "$COCOTRON_SUBDIR/frameworks"
        done

        copy_prebuilt "$SQLITE3_SUBDIR/include" "$SQLITE3_SUBDIR/"
        for SQLITE3_ABI in $PREBUILT_ABIS; do
            copy_prebuilt "$SQLITE3_SUBDIR/libs/$SQLITE3_ABI" "$SQLITE3_SUBDIR/libs"
        done

        for VERSION in $LIBPNG_VERSIONS; do
            copy_prebuilt "$LIBPNG_SUBDIR/$VERSION/include" "$LIBPNG_SUBDIR/$VERSION/"
            for LIBPNG_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$LIBPNG_SUBDIR/$VERSION/libs/$LIBPNG_ABI" "$LIBPNG_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $LIBJPEG_VERSIONS; do
            copy_prebuilt "$LIBJPEG_SUBDIR/$VERSION/include" "$LIBJPEG_SUBDIR/$VERSION/"
            for LIBJPEG_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$LIBJPEG_SUBDIR/$VERSION/libs/$LIBJPEG_ABI" "$LIBJPEG_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $LIBJPEGTURBO_VERSIONS; do
            copy_prebuilt "$LIBJPEGTURBO_SUBDIR/$VERSION/include" "$LIBJPEGTURBO_SUBDIR/$VERSION/"
            for LIBJPEGTURBO_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$LIBJPEGTURBO_SUBDIR/$VERSION/libs/$LIBJPEGTURBO_ABI" "$LIBJPEGTURBO_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $LIBTIFF_VERSIONS; do
            copy_prebuilt "$LIBTIFF_SUBDIR/$VERSION/include" "$LIBTIFF_SUBDIR/$VERSION/"
            for LIBTIFF_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$LIBTIFF_SUBDIR/$VERSION/libs/$LIBTIFF_ABI" "$LIBTIFF_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $ICU_VERSIONS; do
            copy_prebuilt "$ICU_SUBDIR/$VERSION/include" "$ICU_SUBDIR/$VERSION/"
            for ICU_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$ICU_SUBDIR/$VERSION/libs/$ICU_ABI" "$ICU_SUBDIR/$VERSION/libs"
            done
        done

        for VERSION in $BOOST_VERSIONS; do
            copy_prebuilt "$BOOST_SUBDIR/$VERSION/include" "$BOOST_SUBDIR/$VERSION/"
            copy_prebuilt "$BOOST_SUBDIR+icu/$VERSION/include" "$BOOST_SUBDIR+icu/$VERSION/"
            for BOOST_ABI in $PREBUILT_ABIS; do
                copy_prebuilt "$BOOST_SUBDIR/$VERSION/libs/$BOOST_ABI" "$BOOST_SUBDIR/$VERSION/libs"
                copy_prebuilt "$BOOST_SUBDIR+icu/$VERSION/libs/$BOOST_ABI" "$BOOST_SUBDIR+icu/$VERSION/libs"
            done
        done

        if [ -d "$DSTDIR/$COMPILER_RT_SUBDIR" ]; then
            COMPILER_RT_ABIS=$PREBUILT_ABIS
            for COMPILER_RT_ABI in $COMPILER_RT_ABIS; do
                copy_prebuilt "$COMPILER_RT_SUBDIR/libs/$COMPILER_RT_ABI" "$COMPILER_RT_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find compiler-rt source tree!"
        fi

        if [ -d "$DSTDIR/$GCCUNWIND_SUBDIR" ]; then
            GCCUNWIND_ABIS=$PREBUILT_ABIS
            for GCCUNWIND_ABI in $GCCUNWIND_ABIS; do
                copy_prebuilt "$GCCUNWIND_SUBDIR/libs/$GCCUNWIND_ABI" "$GCCUNWIND_SUBDIR/libs"
            done
        else
            echo "WARNING: Could not find libgccunwind source tree!"
        fi
    else
        # Unpack toolchains
        for TC in $TOOLCHAINS; do
            unpack_prebuilt $TC-$SYSTEM "$DSTDIR" "$DSTDIR64"
            echo "Removing sysroot for $TC"
            rm -rf $DSTDIR/toolchains/$TC/prebuilt/$SYSTEM/sysroot
            rm -rf $DSTDIR64/toolchains/$TC/prebuilt/${SYSTEM}_64/sysroot
            rm -rf $DSTDIR64/toolchains/$TC/prebuilt/${SYSTEM}-x86_64/sysroot
        done

        # Unpack clang/llvm
        for LLVM_VERSION in $LLVM_VERSION_LIST; do
            unpack_prebuilt llvm-$LLVM_VERSION-$SYSTEM "$DSTDIR" "$DSTDIR64"
        done

        rm -rf $DSTDIR/toolchains/*l
        rm -rf $DSTDIR64/toolchains/*l

        # Unpack renderscript tools
        unpack_prebuilt renderscript-$SYSTEM "$DSTDIR" "$DSTDIR64"

        # Unpack prebuilt ndk-stack and other host tools
        unpack_prebuilt ndk-stack-$SYSTEM "$DSTDIR" "$DSTDIR64" "yes"
        unpack_prebuilt ndk-depends-$SYSTEM "$DSTDIR" "$DSTDIR64" "yes"
        unpack_prebuilt ndk-make-$SYSTEM "$DSTDIR" "$DSTDIR64"
        unpack_prebuilt ndk-awk-$SYSTEM "$DSTDIR" "$DSTDIR64"
        unpack_prebuilt ndk-perl-$SYSTEM "$DSTDIR" "$DSTDIR64"
        unpack_prebuilt ndk-python-$SYSTEM "$DSTDIR" "$DSTDIR64"
        unpack_prebuilt ndk-yasm-$SYSTEM "$DSTDIR" "$DSTDIR64"

        if [ "$SYSTEM" = "windows" ]; then
            unpack_prebuilt toolbox-$SYSTEM "$DSTDIR" "$DSTDIR64"
        fi
    fi

    # Unpack other host tools
    unpack_prebuilt scan-build-view "$DSTDIR" "$DSTDIR64"

    # Unpack renderscript headers/libs
    unpack_prebuilt renderscript "$DSTDIR" "$DSTDIR64"

    # Unpack misc stuff
    if [ -f "$PREBUILT_DIR/misc.tar.bz2" ]; then
        unpack_prebuilt misc "$DSTDIR" "$DSTDIR64"
    fi

    # Remove duplicated files in case-insensitive file system
    if [ "$SYSTEM" = "windows" -o "$SYSTEM" = "darwin-x86" ]; then
        rm -rf $DSTDIR/tests/build/c++-stl-source-extensions
        rm -rf $DSTDIR64/tests/build/c++-stl-source-extensions
        find "$DSTDIR/platforms" | sort -f | uniq -di | xargs rm -f
        find "$DSTDIR64/platforms" | sort -f | uniq -di | xargs rm -f
    fi

    # Remove include-fixed/linux/a.out.h.   See b.android.com/73728
    find "$DSTDIR/toolchains" "$DSTDIR64/toolchains" -name a.out.h | grep include-fixed/ | xargs rm -f

    # Remove redundant pretty-printers/libstdcxx
    rm -rf $DSTDIR/prebuilt/*/share/pretty-printers/libstdcxx/gcc-l*
    rm -rf $DSTDIR/prebuilt/*/share/pretty-printers/libstdcxx/gcc-4.9-*
    rm -rf $DSTDIR64/prebuilt/*/share/pretty-printers/libstdcxx/gcc-l*
    rm -rf $DSTDIR64/prebuilt/*/share/pretty-printers/libstdcxx/gcc-4.9-*

    # Remove python tests
    find $DSTDIR/prebuilt/*/lib/python* -name test -exec rm -rf {} \;
    find $DSTDIR64/prebuilt/*/lib/python* -name test -exec rm -rf {} \;

    # Remove python *.pyc and *.pyo files
    find $DSTDIR/prebuilt/*/lib/python* -name "*.pyc" -exec rm -rf {} \;
    find $DSTDIR/prebuilt/*/lib/python* -name "*.pyo" -exec rm -rf {} \;
    find $DSTDIR64/prebuilt/*/lib/python* -name "*.pyc"  -exec rm -rf {} \;
    find $DSTDIR64/prebuilt/*/lib/python* -name "*.pyo"  -exec rm -rf {} \;

    # Remove .git*
    find $DSTDIR -name ".git*" -exec rm -rf {} \;
    find $DSTDIR64 -name ".git*" -exec rm -rf {} \;

    # unpack vendor utils
    #echo "$SCRIPTS_DIR/install-vendor-utils --system=$SYSTEM --out32-dir=$DSTDIR --out64-dir=$DSTDIR64"
    #$SCRIPTS_DIR/install-vendor-utils --system="$SYSTEM" --out32-dir="$DSTDIR" --out64-dir="$DSTDIR64"
    #fail_panic "Could not install vendor utils"
    #echo "$SCRIPTS_DIR/install-crew --out-dir=$DSTDIR/tools"
    #$SCRIPTS_DIR/install-crew --out-dir="$DSTDIR/tools"
    #fail_panic "Could not install CREW"

    # Create an archive for the final package. Extension depends on the
    # host system.
    ARCHIVE=$BIN_RELEASE
    if [ "$TRY64" = "yes" ]; then
        ARCHIVE=`name64 $ARCHIVE`
    elif [ "$SYSTEM" = "windows" ]; then
        ARCHIVE=$ARCHIVE-x86
    fi
     case "$SYSTEM" in
        windows)
            ARCHIVE64="${ARCHIVE}_64.exe"
            ARCHIVE="${ARCHIVE}.exe"
            SHORT_SYSTEM="windows"
            ;;
        *)
            ARCHIVE64="${ARCHIVE}_64.bin"
            ARCHIVE="${ARCHIVE}.bin"
            SHORT_SYSTEM=$SYSTEM
            ;;
    esac
    if [ "$TRY64" = "yes" ]; then
        ARCHIVE=$ARCHIVE64
    fi
    # make all file universally readable, and all executable (including directory)
    # universally executable, punt intended
    find $DSTDIR $DSTDIR64 -exec chmod a+r {} \;
    find $DSTDIR $DSTDIR64 -executable -exec chmod a+x {} \;
    echo "Creating $ARCHIVE"
    pack_release "$OUT_DIR/$ARCHIVE" "$TMPDIR" "$RELEASE_VERSION"
    fail_panic "Could not create archive: $OUT_DIR/$ARCHIVE"
    if [ "$SEPARATE_64" = "yes" ] ; then
        rm -rf "$DSTDIR/prebuilt/common"
        rm -rf "$DSTDIR/prebuilt/$SHORT_SYSTEM"
        find "$DSTDIR/toolchains" -type d -name prebuilt | xargs rm -rf
        cp -r "$DSTDIR64"/* "$DSTDIR"/
        rm -rf "$DSTDIR64"
        echo "Creating $ARCHIVE64"
        pack_release "$OUT_DIR/$ARCHIVE64" "$TMPDIR" "$RELEASE_VERSION"
        fail_panic "Could not create archive: $OUT_DIR/$ARCHIVE64"
    fi
done

echo "Cleaning up."
rm -rf $TMPDIR/reference
rm -rf $TMPDIR/prev-ndk

echo "Done, please see packages in $OUT_DIR:"
ls -l $OUT_DIR
