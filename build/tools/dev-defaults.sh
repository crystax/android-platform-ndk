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
PREBUILT_ABIS="armeabi armeabi-v7a x86"

# Location of the crystax sources, relative to the NDK root directory
CRYSTAX_SUBDIR=sources/crystax

# Location of the STLport sources, relative to the NDK root directory
STLPORT_SUBDIR=sources/cxx-stl/stlport

# Location of the GAbi++ sources, relative to the NDK root directory
GABIXX_SUBDIR=sources/cxx-stl/gabi++

# Location of the GNU libstdc++ headers and libraries, relative to the NDK
# root directory.
GNUSTL_SUBDIR=sources/cxx-stl/gnu-libstdc++

# The date to use when downloading toolchain sources from AOSP servers
# Leave it empty for tip of tree.
TOOLCHAIN_GIT_DATE=2011-02-23

SUPPORTED_GCC_VERSIONS="4.4.3 4.6.3"
DEFAULT_GCC_VERSION=4.4.3

DEFAULT_BINUTILS_VERSION_FOR_GCC_4_4_3=2.19
DEFAULT_BINUTILS_VERSION_FOR_GCC_4_6_3=2.20.1
DEFAULT_GDB_VERSION_FOR_GCC_4_4_3=6.6
DEFAULT_GDB_VERSION_FOR_GCC_4_6_3=7.3
DEFAULT_MPFR_VERSION_FOR_GCC_4_4_3=2.4.1
DEFAULT_MPFR_VERSION_FOR_GCC_4_6_3=3.0.1
DEFAULT_GMP_VERSION_FOR_GCC_4_4_3=4.2.4
DEFAULT_GMP_VERSION_FOR_GCC_4_6_3=5.0.2
DEFAULT_MPC_VERSION_FOR_GCC_4_4_3=0.8.1
DEFAULT_MPC_VERSION_FOR_GCC_4_6_3=0.9
DEFAULT_EXPAT_VERSION_FOR_GCC_4_4_3=2.0.1
DEFAULT_EXPAT_VERSION_FOR_GCC_4_6_3=2.0.1
DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_4_4_3=0.15.9
DEFAULT_CLOOG_PPL_VERSION_FOR_GCC_4_6_3=0.15.9
DEFAULT_PPL_VERSION_FOR_GCC_4_4_3=0.11.2
DEFAULT_PPL_VERSION_FOR_GCC_4_6_3=0.11.2

# The list of default CPU architectures we support
DEFAULT_ARCHS="arm x86"

# Default toolchain names and prefix
#
# This is used by get_default_toolchain_name_for_arch and get_default_toolchain_prefix_for_arch
# defined below
DEFAULT_ARCH_TOOLCHAIN_4_4_3_arm=arm-linux-androideabi-4.4.3
DEFAULT_ARCH_TOOLCHAIN_4_6_3_arm=arm-linux-androideabi-4.6.3
DEFAULT_ARCH_TOOLCHAIN_PREFIX_arm=arm-linux-androideabi

DEFAULT_ARCH_TOOLCHAIN_4_4_3_x86=x86-4.4.3
DEFAULT_ARCH_TOOLCHAIN_4_6_3_x86=x86-4.6.3
DEFAULT_ARCH_TOOLCHAIN_PREFIX_x86=i686-android-linux

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
        4.6.3)
            V=4_6_3
            ;;
        *)
            echo "ERROR: Unsupported GCC version: $1, use one of: $SUPPORTED_GCC_VERSIONS" 1>&2
            exit 1
            ;;
    esac
    echo "$V"
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
        *)
            2> echo "ERROR: Unsupported architecture name: $1, use one of: arm x86"
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
        *)
            2> echo "ERROR: Unsupported architecture name: $1, use one of: arm x86"
            exit 1
            ;;
    esac
    echo "$RET"
}

# Return the toolchain name for a given gcc version and architecture
# $1: GCC version
# $2: Architecture name
# Out: arch-specific toolchain name (e.g. arm-linux-androideabi-$GCC_VERSION)
# Return empty for unknown gcc version or arch
get_toolchain_name_for_gcc_and_arch()
{
    local V
    V=$(get_plain_gcc_version $1)
    eval echo "\$DEFAULT_ARCH_TOOLCHAIN_${V}_$2"
}

# Return the default name for a given architecture
# $1: Architecture name
# Out: default arch-specific toolchain name (e.g. arm-linux-androideabi-$GCC_VERSION)
# Return empty for unknown arch
get_default_toolchain_name_for_arch ()
{
    get_toolchain_name_for_gcc_and_arch $DEFAULT_GCC_VERSION $1
}

# Return the default toolchain program prefix for a given architecture
# $1: Architecture name
# Out: default arch-specific toolchain prefix (e.g. arm-linux-androideabi)
# Return empty for unknown arch
get_default_toolchain_prefix_for_arch ()
{
    eval echo "\$DEFAULT_ARCH_TOOLCHAIN_PREFIX_$1"
}

