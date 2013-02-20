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
TOOLCHAIN_GIT_DATE=now

# The space-separated list of all GCC versions we support in this NDK
DEFAULT_GCC_VERSION_LIST="4.7 4.6 4.4.3"

# The default GCC version for this NDK, i.e. the first item in
# $DEFAULT_GCC_VERSION_LIST
#
DEFAULT_GCC_VERSION=$(echo "$DEFAULT_GCC_VERSION_LIST" | tr ' ' '\n' | head -n 1)

DEFAULT_BINUTILS_VERSION=2.21
DEFAULT_BINUTILS_VERSION_FOR_arm_GCC_4_4_3=2.19
DEFAULT_BINUTILS_VERSION_FOR_x86_GCC_4_4_3=2.19
DEFAULT_BINUTILS_VERSION_FOR_arm_GCC_4_7=2.22
DEFAULT_BINUTILS_VERSION_FOR_x86_GCC_4_7=2.22
DEFAULT_BINUTILS_VERSION_FOR_mips_GCC_4_7=2.22

DEFAULT_GDB_VERSION=7.5
DEFAULT_GDB_VERSION_FOR_GCC_4_4_3=6.6

DEFAULT_MPFR_VERSION=3.1.1
DEFAULT_MPFR_VERSION_FOR_GCC_4_4_3=2.4.1

DEFAULT_GMP_VERSION=5.0.5
DEFAULT_GMP_VERSION_FOR_GCC_4_4_3=4.2.4

DEFAULT_MPC_VERSION=1.0.1
DEFAULT_MPC_VERSION_FOR_GCC_4_4_3=0.8.1

DEFAULT_EXPAT_VERSION=2.0.1

DEFAULT_CLOOG_VERSION=0.17.0

DEFAULT_PPL_VERSION=1.0

DEFAULT_PYTHON_VERSION=2.7.3

DEFAULT_PERL_VERSION=5.16.2

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
DEFAULT_LLVM_VERSION_LIST="3.2 3.1"

# The default LLVM version (first item in the list)
DEFAULT_LLVM_VERSION=$(echo "$DEFAULT_LLVM_VERSION_LIST" | tr ' ' '\n' | head -n 1)

# The default URL to download the LLVM tar archive
DEFAULT_LLVM_URL="http://llvm.org/releases"

# The list of default host NDK systems we support
DEFAULT_SYSTEMS="linux-x86 windows darwin-x86"

# Return modified "plain" version for a given toolchain
# $1: Toolchain name (e.g. arm-linux-androideabi-4.4.3) or GCC version
# Out: plain version (e.g. 4_4_3)
get_plain_toolchain_version()
{
    echo $1 | sed -e 's/^.*-\([0-9.]\+.*\)$/\1/' | sed -e 's/[^A-Za-z0-9_]/_/g'
}

# Return default binutils version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: binutils version
get_default_binutils_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_BINUTILS_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_BINUTILS_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET="$DEFAULT_BINUTILS_VERSION"
    if [ -z "$RET" ] ; then
        echo "ERROR: No default binutils version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default GDB version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: GDB version
get_default_gdb_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_GDB_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_GDB_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_GDB_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default gdb version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default GMP version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: GMP version
get_default_gmp_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_GMP_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_GMP_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_GMP_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default gmp version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default MPFR version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: MPFR version
get_default_mpfr_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_MPFR_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_MPFR_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_MPFR_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default mpfr version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default MPC version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: MPC version
get_default_mpc_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_MPC_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_MPC_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_MPC_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default mpc version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default EXPAT version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: EXPAT version
get_default_expat_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_EXPAT_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_EXPAT_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_EXPAT_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default expat version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default CLOOG version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: CLOOG version
get_default_cloog_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_CLOOG_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_CLOOG_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_CLOOG_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default cloog version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return default PPL version for a given toolchain
# $1: Toolchain name or prefix (e.g. arm-linux-androideabi-4.4.3)
# Out: PPL version
get_default_ppl_version()
{
    local V A RET
    V=$(get_plain_toolchain_version $1)
    A=$(get_arch_for_toolchain $1)
    eval RET="\$DEFAULT_PPL_VERSION_FOR_${A}_GCC_${V}"
    [ -z "$RET" ] && eval RET="\$DEFAULT_PPL_VERSION_FOR_GCC_${V}"
    [ -z "$RET" ] && RET=$DEFAULT_PPL_VERSION
    if [ -z "$RET" ] ; then
        echo "ERROR: No default ppl version found for $1" 1>&2
        exit 1
    fi
    echo $RET
}

# Return architecture name for a given toolchain name
# $1: Toolchain name (e.g. arm-linux-androideabi-4.4.3, x86-4.6 etc)
# Out: Architecture name
get_arch_for_toolchain ()
{
    local RET
    case $1 in
        arm*)
            RET=arm
            ;;
        x86*|i686-*)
            RET=x86
            ;;
        mips*)
            RET=mips
            ;;
        *)
            RET=unknown
            ;;
    esac
    echo "$RET"
}

# The default issue tracker URL
DEFAULT_ISSUE_TRACKER_URL="http://source.android.com/source/report-bugs.html"

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
            echo "ERROR: Unsupported architecture name: $1, use one of: arm x86 mips" 1>&2
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
            echo "ERROR: Unsupported architecture name: $1, use one of: arm x86 mips" 1>&2
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
# $2: toolchain versions list (optional, delimited by commas)
# Out: list of toolchain names for this arch (e.g. arm-linux-androideabi-4.6 arm-linux-androideabi-4.4.3)
# Return empty for unknown arch
get_toolchain_name_list_for_arch ()
{
    local PREFIX VERSION_LIST VERSION RET
    PREFIX=$(eval echo \"\$DEFAULT_ARCH_TOOLCHAIN_NAME_$1\")
    if [ -z "$PREFIX" ]; then
        return 0
    fi
    VERSION_LIST=$(commas_to_spaces $2)
    [ -z "$VERSION_LIST" ] && VERSION_LIST=$DEFAULT_GCC_VERSION_LIST
    for VERSION in $VERSION_LIST; do
        RET=$RET" $PREFIX-$VERSION"
    done
    RET=${RET## }
    echo "$RET"
}

