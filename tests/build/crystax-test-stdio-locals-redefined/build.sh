#!/bin/bash

. $NDK/build/tools/dev-defaults.sh

case $(uname -s | tr '[A-Z]' '[a-z]') in
    darwin)
        HOST_ARCH=$(uname -m)
        HOST_TAG=darwin-$HOST_ARCH
        ;;
    linux)
        HOST_ARCH=$(uname -m)
        case $HOST_ARCH in
            i?86)
                HOST_ARCH=x86
                ;;
            x86_64)
                file -b /bin/ls | grep -q 32-bit && HOST_ARCH=x86
                ;;
            *)
                echo "ERROR: Unsupported host CPU architecture: '$HOST_ARCH'" 1>&2
                exit 1
        esac
        HOST_TAG=linux-$HOST_ARCH
        ;;
    *)
        echo "WARNING: This test cannot run on this machine!" 1>&2
        exit 0
        ;;
esac

SYMBOLS="
__fcloseall
__fflush
__fgetwc_mbs
__fputwc
__fread
__sclose
__sdidinit
__sflags
__sflush
__sfp
__sglue
__sinit
__slbexpand
__smakebuf
__sread
__srefill
__srget
__sseek
__svfscanf
__swbuf
__swhatbuf
__swrite
__swsetup
__ungetc
__ungetwc
__vfprintf
__vfscanf
__vfwprintf
__vfwscanf
_cleanup
_fseeko
_ftello
_fwalk
_sread
_sseek
_swrite
"

check_libcrystax()
{
    local sym
    local nm=$1
    local lib=$2
    if [ -z "$nm" -o -z "$lib" ]; then
        echo "ERROR: Usage: $0 nm libcrystax" 1>&2
        exit 1
    fi

    if [ ! -f $lib ]; then
        echo "ERROR: No such file: $lib" 1>&2
        exit 1
    fi

    echo "Checking $lib ..."

    for sym in $SYMBOLS; do
        $nm $lib 2>/dev/null | grep -q "^[^ ]* T ${sym}$"
        if [ $? -eq 0 ]; then
            echo "ERROR: Symbol ${sym} defined in $lib even though it should be redefined to __crystax_${sym}" 1>&2
            exit 1
        fi
    done
}

for ABI in $(ls -1 $NDK/sources/crystax/libs | sort); do
    case $ABI in
        armeabi*)
            ARCH=arm
            ;;
        arm64-v8a)
            ARCH=arm64
            ;;
        *)
            ARCH=$ABI
    esac

    TOOLCHAIN_NAME=$(get_default_toolchain_name_for_arch $ARCH)
    TOOLCHAIN_PREFIX=$(get_default_toolchain_prefix_for_arch $ARCH)

    NM=$NDK/toolchains/$TOOLCHAIN_NAME/prebuilt/$HOST_TAG/bin/${TOOLCHAIN_PREFIX}-nm

    if [ ! -f "$NM" ]; then
        echo "ERROR: Missing binary: $NM" 1>&2
        exit 1
    fi

    for LIBCRYSTAX in $(find $NDK/sources/crystax/libs/$ABI -name 'libcrystax.*' -print); do
        check_libcrystax $NM $LIBCRYSTAX
    done
done

echo "Done!"
