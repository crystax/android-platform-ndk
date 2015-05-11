#!/bin/bash

. $NDK/build/tools/dev-defaults.sh

HOST_OS=$(uname -s | tr '[A-Z]' '[a-z]')
case $HOST_OS in
    cygwin*|mingw*)
        HOST_OS=windows
esac

HOST_ARCH=""
HOST_ARCH2=""

case $HOST_OS in
    darwin|linux|windows)
        HOST_ARCH=$(uname -m)
        HOST_ARCH2=
        case $HOST_ARCH in
            i?86)
                HOST_ARCH=x86
                ;;
            x86_64)
                if [ "$HOST_OS" = "linux" ]; then
                    if file -b /bin/ls | grep -q 32-bit; then
                        HOST_ARCH=x86
                    else
                        HOST_ARCH2=x86
                    fi
                else
                    HOST_ARCH2=x86
                fi
                ;;
            *)
                echo "ERROR: Unsupported host CPU architecture: '$HOST_ARCH'" 1>&2
                exit 1
        esac
        HOST_TAG=${HOST_OS}-${HOST_ARCH}
        test -n "$HOST_ARCH2" && HOST_TAG2=${HOST_OS}-${HOST_ARCH2}
        ;;
    *)
        echo "ERROR: Unsupported host OS: '$HOST_OS'" 1>&2
        exit 1
        ;;
esac

if [ "$HOST_OS" = "windows" -a "$HOST_ARCH" = "x86" ]; then
    HOST_TAG=$HOST_OS
else
    HOST_TAG=${HOST_OS}-${HOST_ARCH}
fi

HOST_TAG2=""
if [ -n "$HOST_ARCH2" ]; then
    if [ "$HOST_OS" = "windows" -a "$HOST_ARCH2" = "x86" ]; then
        HOST_TAG2=$HOST_OS
    else
        HOST_TAG2=${HOST_OS}-${HOST_ARCH2}
    fi
fi

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

    tmpfile=/tmp/libcrystax-symbols-$$.txt

    $nm $lib >$tmpfile 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "ERROR: Can't get symbols from $lib" 1>&2
        rm -f $tmpfile
        exit 1
    fi

    for sym in $SYMBOLS; do
        grep -q "^[^ ]* T ${sym}$" $tmpfile
        if [ $? -eq 0 ]; then
            echo "ERROR: Symbol ${sym} defined in $lib even though it should be redefined to __crystax_${sym}" 1>&2
            rm -f $tmpfile
            exit 1
        fi
    done

    rm -f $tmpfile
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

    for tag in $HOST_TAG $HOST_TAG2; do
        NM=$NDK/toolchains/$TOOLCHAIN_NAME/prebuilt/$tag/bin/${TOOLCHAIN_PREFIX}-nm
        echo "Probing for ${NM}..."
        if [ -x $NM ]; then
            echo "Found: $NM"
            break
        fi
    done

    if [ ! -x "$NM" ]; then
        echo "ERROR: Can't find $ARCH nm" 1>&2
        exit 1
    fi

    for LIBCRYSTAX in $(find $NDK/sources/crystax/libs/$ABI -name 'libcrystax.*' -print); do
        check_libcrystax $NM $LIBCRYSTAX
    done
done

echo "Done!"
