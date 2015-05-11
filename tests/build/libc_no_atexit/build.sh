#!/bin/bash

# Check that the libc.so for all platforms, and all architectures
# Does not export 'atexit' and '__dso_handle' symbols.
#

export ANDROID_NDK_ROOT=$NDK

NDK_BUILDTOOLS_PATH=$NDK/build/tools
. $NDK/build/tools/prebuilt-common.sh
echo DEFAULT_ARCHS=$DEFAULT_ARCHS

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
        ;;
    *)
        echo "ERROR: Unsupported host OS: '$HOST_OS'" 1>&2
        exit 1
esac

if [ "$HOST_ARCH" = "x86" -a "$HOST_OS" = "windows" ]; then
    HOST_TAG=${HOST_OS}
else
    HOST_TAG=${HOST_OS}-${HOST_ARCH}
fi

HOST_TAG2=""
if [ -n "$HOST_ARCH2" ]; then
    if [ "$HOST_ARCH2" = "x86" -a "$HOST_OS" = "windows" ]; then
        HOST_TAG2=$HOST_OS
    else
        HOST_TAG2=${HOST_OS}-${HOST_ARCH2}
    fi
fi

FAILURE=
COUNT=0

TMPFILE=/tmp/libc-check-$$.txt
trap "rm -f $TMPFILE" EXIT INT QUIT ABRT TERM

for ARCH in $DEFAULT_ARCHS; do
    TOOLCHAIN_NAME=$(get_default_toolchain_name_for_arch $ARCH)
    TOOLCHAIN_PREFIX=$(get_default_toolchain_prefix_for_arch $ARCH)

    READELF=
    for tag in $HOST_TAG $HOST_TAG2; do
        READELF=$NDK/toolchains/$TOOLCHAIN_NAME/prebuilt/$tag/bin/${TOOLCHAIN_PREFIX}-readelf
        if [ -x $READELF ]; then
            break
        fi
    done

    if [ ! -x "$READELF" ]; then
        echo "ERROR: Can't find $ARCH readelf" 1>&2
        exit 1
    fi

    LIBRARIES=$(cd $NDK && find platforms -name "libc.so" | sed -e 's!^!'$NDK'/!' | grep arch-$ARCH)
    for LIB in $LIBRARIES; do
        COUNT=$(( $COUNT + 1 ))

        echo "Checking: $LIB"

        $READELF -s $LIB >$TMPFILE
        if [ $? -ne 0 ]; then
            echo "ERROR: readelf $LIB failed" 1>&2
            FAILURE=true
            continue
        fi

        for SYM in atexit __dso_handle; do
            if grep -q -F " $SYM" $TMPFILE; then
                echo "ERROR: $NDK/$LIB exposes '$SYM'!" 1>&2
                FAILURE=true
            fi
        done
    done
done

rm -f $TMPFILE

if [ "$COUNT" = 0 ]; then
    echo "ERROR: Did not find any libc.so in $NDK/platforms!" 1>&2
    exit 1
fi

if [ "$FAILURE" = "true" ]; then
    exit 1
else
    echo "All $COUNT libc.so are ok!"
    exit 0
fi
