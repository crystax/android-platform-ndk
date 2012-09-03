#!/bin/bash
# Default values used by several dev-scripts.
#

# Current list of platform levels we support
#
# Note: levels 6 and 7 are omitted since they have the same native
# APIs as level 5. Same for levels 10, 11 and 12
#
API_LEVELS="3 4 5 8 9 14"

# Default ABIs for the target prebuilt binaries.
PREBUILT_ABIS="armeabi armeabi-v7a x86 mips"

# Location of the crystax sources, relative to the NDK root directory
CRYSTAX_SUBDIR=sources/crystax

# Location of the STLport sources, relative to the NDK root directory
STLPORT_SUBDIR=sources/cxx-stl/stlport

# Location of the GAbi++ sources, relative to the NDK root directory
GABIXX_SUBDIR=sources/cxx-stl/gabi++

# Location of the GNU libstdc++ headers and libraries, relative to the NDK
# root directory.
GNUSTL_SUBDIR=sources/cxx-stl/gnu-libstdc++

# Location of the GNU libobjc headers and libraries, relative to the NDK
# root directory.
GNUOBJC_SUBDIR=sources/objc/gnu-libobjc

# The date to use when downloading toolchain sources from AOSP servers
# Leave it empty for tip of tree.
TOOLCHAIN_GIT_DATE=2012-08-31

# The space-separated list of all GCC versions we support in this NDK
DEFAULT_GCC_VERSION_LIST="4.6 4.7 4.4.3"

# The default GCC version for this NDK, i.e. the first item in
# $DEFAULT_GCC_VERSION_LIST
#
DEFAULT_GCC_VERSION=$(echo "$DEFAULT_GCC_VERSION_LIST" | tr ' ' '\n' | head -n 1)

DEFAULT_BINUTILS_VERSION=2.21
DEFAULT_GDB_VERSION=7.3.x
DEFAULT_MPFR_VERSION=2.4.1
DEFAULT_GMP_VERSION=5.0.5
DEFAULT_MPC_VERSION=0.8.1

#DEFAULT_BINUTILS_VERSION_FOR_GCC_4_4_3=2.19
#DEFAULT_BINUTILS_VERSION_FOR_GCC_4_6_4=2.22
#DEFAULT_BINUTILS_VERSION_FOR_GCC_4_7_2=2.22
#DEFAULT_GDB_VERSION_FOR_GCC_4_4_3=6.6
#DEFAULT_GDB_VERSION_FOR_GCC_4_6_4=7.4
#DEFAULT_GDB_VERSION_FOR_GCC_4_7_2=7.4
#DEFAULT_MPFR_VERSION_FOR_GCC_4_4_3=2.4.1
#DEFAULT_MPFR_VERSION_FOR_GCC_4_6_4=3.0.1
#DEFAULT_MPFR_VERSION_FOR_GCC_4_7_2=3.0.1
#DEFAULT_GMP_VERSION_FOR_GCC_4_4_3=4.2.4
#DEFAULT_GMP_VERSION_FOR_GCC_4_6_4=5.0.2
#DEFAULT_GMP_VERSION_FOR_GCC_4_7_2=5.0.2
#DEFAULT_MPC_VERSION_FOR_GCC_4_4_3=0.8.1
#DEFAULT_MPC_VERSION_FOR_GCC_4_6_4=0.9
#DEFAULT_MPC_VERSION_FOR_GCC_4_7_2=0.9
#DEFAULT_EXPAT_VERSION_FOR_GCC_4_4_3=2.0.1
#DEFAULT_EXPAT_VERSION_FOR_GCC_4_6_4=2.0.1
#DEFAULT_EXPAT_VERSION_FOR_GCC_4_7_2=2.0.1
#DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_4_4_3=0.15.9
#DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_4_6_4=0.15.9
#DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_4_7_2=0.15.9
#DEFAULT_PPL_VERSION_FOR_GCC_4_4_3=0.11.2
#DEFAULT_PPL_VERSION_FOR_GCC_4_6_4=0.11.2
#DEFAULT_PPL_VERSION_FOR_GCC_4_7_2=0.11.2

# The list of default CPU architectures we support
DEFAULT_ARCHS="arm x86 mips"

# Default toolchain names and prefix
#
# This is used by get_default_toolchain_name_for_arch and get_default_toolchain_prefix_for_arch
# defined below
DEFAULT_ARCH_TOOLCHAIN_NAME_arm=arm-linux-androideabi
DEFAULT_ARCH_TOOLCHAIN_PREFIX_arm=arm-linux-androideabi

DEFAULT_ARCH_TOOLCHAIN_NAME_x86=x86
DEFAULT_ARCH_TOOLCHAIN_PREFIX_x86=i686-linux-android

DEFAULT_ARCH_TOOLCHAIN_NAME_mips=mipsel-linux-android
DEFAULT_ARCH_TOOLCHAIN_PREFIX_mips=mipsel-linux-android

# The space-separated list of all LLVM versions we support in NDK
DEFAULT_LLVM_VERSION_LIST="3.1"

# The default URL to download the LLVM tar archive
DEFAULT_LLVM_URL="http://llvm.org/releases"

# The list of default host NDK systems we support
DEFAULT_SYSTEMS="linux-x86 windows darwin-x86"

# Return modified "plain" variant for a given GCC version
# $1: GCC version
# Out: plain version (e.g. 4_4_3)
get_plain_gcc_version()
{
    local V
    case $1 in
        4.4.3)
            V=4_4_3
            ;;
        4.6.4)
            V=4_6_4
            ;;
        4.7.2)
            V=4_7_2
            ;;
        *)
            echo "ERROR: Unsupported GCC version: $1, use one of: $SUPPORTED_GCC_VERSIONS" 1>&2
            exit 1
            ;;
    esac
    echo "$V"
}

# Return gcc source directory for given version
# $1: GCC version
# Out: gcc source directory name (example: gcc-4.4.3, gcc-4.6.x)
get_gcc_source_directory()
{
    if [ "x$1" = "x4.4.3" ]; then
        echo gcc-$1
    else
        echo gcc-$(echo $1 | sed -e 's/\.[0-9]\+$/.x/')
    fi
}

# Return default binutils version for a given GCC version
# $1: GCC version
# Out: binutils version
get_default_binutils_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_BINUTILS_VERSION_FOR_GCC_$V"
}

# Return default GDB version for a given GCC version
# $1: GCC version
# Out: GDB version
get_default_gdb_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_GDB_VERSION_FOR_GCC_$V"
}

# Return default GMP version for a given GCC version
# $1: GCC version
# Out: GMP version
get_default_gmp_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_GMP_VERSION_FOR_GCC_$V"
}

# Return default MPFR version for a given GCC version
# $1: GCC version
# Out: MPFR version
get_default_mpfr_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_MPFR_VERSION_FOR_GCC_$V"
}

# Return default MPC version for a given GCC version
# $1: GCC version
# Out: MPC version
get_default_mpc_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_MPC_VERSION_FOR_GCC_$V"
}

# Return default EXPAT version for a given GCC version
# $1: GCC version
# Out: EXPAT version
get_default_expat_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_EXPAT_VERSION_FOR_GCC_$V"
}

# Return default CLOOG-PPL version for a given GCC version
# $1: GCC version
# Out: CLOOG-PPL version
get_default_cloog_ppl_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_$V"
}

# Return default PPL version for a given GCC version
# $1: GCC version
# Out: PPL version
get_default_ppl_version_for_gcc()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_PPL_VERSION_FOR_GCC_$V"
}

# Return default NDK ABI for a given architecture name
# $1: Architecture name
# Out: ABI name
get_default_abi_for_arch ()
{
    local RET
    case $1 in
        arm)
            RET="armeabi"
            ;;
        x86)
            RET="x86"
            ;;
        mips)
            RET="mips"
            ;;
        *)
            2> echo "ERROR: Unsupported architecture name: $1, use one of: arm x86 mips"
            exit 1
            ;;
    esac
    echo "$RET"
}


# Retrieve the list of default ABIs supported by a given architecture
# $1: Architecture name
# Out: space-separated list of ABI names
get_default_abis_for_arch ()
{
    local RET
    case $1 in
        arm)
            RET="armeabi armeabi-v7a"
            ;;
        x86)
            RET="x86"
            ;;
        mips)
            RET="mips"
            ;;
        *)
            2> echo "ERROR: Unsupported architecture name: $1, use one of: arm x86 mips"
            exit 1
            ;;
    esac
    echo "$RET"
}

# Return toolchain name for given architecture and GCC version
# $1: Architecture name (e.g. 'arm')
# $2: optional, GCC version (e.g. '4.6')
# Out: default arch-specific toolchain name (e.g. 'arm-linux-androideabi-$GCC_VERSION')
# Return empty for unknown arch
get_toolchain_name_for_arch ()
{
    if [ ! -z "$2" ] ; then
        eval echo \"\${DEFAULT_ARCH_TOOLCHAIN_NAME_$1}-$2\"
    else
        eval echo \"\${DEFAULT_ARCH_TOOLCHAIN_NAME_$1}\"
    fi
}

# Return the default toolchain name for a given architecture
# $1: Architecture name (e.g. 'arm')
# Out: default arch-specific toolchain name (e.g. 'arm-linux-androideabi-$DEFAULT_GCC_VERSION')
# Return empty for unknown arch
get_default_toolchain_name_for_arch ()
{
    eval echo \"\${DEFAULT_ARCH_TOOLCHAIN_NAME_$1}-$DEFAULT_GCC_VERSION\"
}

# Return the default toolchain program prefix for a given architecture
# $1: Architecture name
# Out: default arch-specific toolchain prefix (e.g. arm-linux-androideabi)
# Return empty for unknown arch
get_default_toolchain_prefix_for_arch ()
{
    eval echo "\$DEFAULT_ARCH_TOOLCHAIN_PREFIX_$1"
}

# Get the list of all toolchain names for a given architecture
# $1: architecture (e.g. 'arm')
# Out: list of toolchain names for this arch (e.g. arm-linux-androideabi-4.6 arm-linux-androideabi-4.4.3)
# Return empty for unknown arch
get_toolchain_name_list_for_arch ()
{
    local PREFIX VERSION RET
    PREFIX=$(eval echo \"\$DEFAULT_ARCH_TOOLCHAIN_NAME_$1\")
    if [ -z "$PREFIX" ]; then
        return 0
    fi
    RET=""
    for VERSION in $DEFAULT_GCC_VERSION_LIST; do
        RET=$RET" $PREFIX-$VERSION"
    done
    RET=${RET## }
    echo "$RET"
}

# Return the binutils version to be used by default when
# building a given version of GCC. This is needed to ensure
# we use binutils-2.19 when building gcc-4.4.3 for ARM and x86,
# and binutils-2.21 in other cases (mips, or gcc-4.6).
#
# Note that technically, we could use 2.21 for all versions of
# GCC, however, in NDK r7, we did build GCC 4.4.3 with binutils-2.20.1
# and this resulted in weird C++ debugging bugs. For NDK r7b and higher,
# binutils was reverted to 2.19, to ensure at least
# feature/bug compatibility.
#
# $1: toolchain with version numer (e.g. 'arm-linux-androideabi-4.6')
#
get_default_binutils_version_for_gcc ()
{
    case $1 in
        arm-*-4.4.3|x86-4.4.3|x86-*-4.4.3) echo "2.19";;
        *) echo "$DEFAULT_BINUTILS_VERSION";;
    esac
}
