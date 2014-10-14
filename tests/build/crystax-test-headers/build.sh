#!/bin/bash

cd $(dirname $0) || exit 1
MYDIR=$(pwd)

if [ -z "$NDK" ]; then
    NDK=$(cd ../../.. && pwd)
fi

INCDIR=$NDK/sources/crystax/include
if [ ! -d $INCDIR ]; then
    echo "ERROR: Can't find CrystaX include directory" 1>&2
    exit 1
fi

if [ -z "$PLATFORMS" ]; then
    APILEVELS=$(cd $NDK/platforms && ls -d android-* | sed 's,^android-,,' | sed 's,L,,' | sort -n)
    test -d $NDK/platforms/android-L && APILEVELS="$APILEVELS L"
    PLATFORMS=$(for a in $APILEVELS; do echo "android-$a"; done)
else
    PLATFORMS=$(echo $PLATFORMS | sed 's/,/ /g')
fi

HEADERS=$( { find $INCDIR -name '*.h' -print; find $INCDIR -name '*.hpp' -print; } | sort)

abis_for_platform()
{
    local platform=$1
    if [ -z "$platform" ]; then
        echo "ERROR: You should pass android-N to call of abis_for_platform" 1>&2
        exit 1
    fi

    local apilevel=$(expr "$platform" : "^android-\(.*\)")
    if [ "$apilevel" = "L" ]; then
        echo all
    elif [ $apilevel -ge 21 ]; then
        echo all
    elif [ $apilevel -ge 9 ]; then
        echo armeabi armeabi-v7a armeabi-v7a-hard x86 mips
    else
        echo armeabi armeabi-v7a armeabi-v7a-hard
    fi
}

run()
{
    echo "## COMMAND: $@"
    "$@"
}

for HEADER in $HEADERS; do
    HEADER=${HEADER##$INCDIR/}

    # Skip internal headers
    [[ ${HEADER} == "_ctype.h"    ]] && continue
    [[ ${HEADER##crystax/arm64/}   != $HEADER ]] && continue
    [[ ${HEADER##crystax/details}  != $HEADER ]] && continue
    [[ ${HEADER##crystax/freebsd/} != $HEADER ]] && continue
    [[ ${HEADER##crystax/mips64/}  != $HEADER ]] && continue
    [[ ${HEADER##crystax/sys/}     != $HEADER ]] && continue
    [[ ${HEADER##linux/}           != $HEADER ]] && continue
    [[ ${HEADER##machine/}         != $HEADER ]] && continue
    [[ ${HEADER##sys/_}            != $HEADER ]] && continue
    [[ ${HEADER##xlocale/}         != $HEADER ]] && continue

    for PLATFORM in $PLATFORMS; do
        for ABI in $(abis_for_platform $PLATFORM); do
            echo ""
            echo "==================================================================================="
            echo "===> Compiling CrystaX header $HEADER for $PLATFORM/$ABI <==="
            echo "==================================================================================="
            echo ""
            run $NDK/ndk-build -C $MYDIR -B APP_CFLAGS=-DHEADER=\"\<$HEADER\>\" APP_PLATFORM=$PLATFORM APP_ABI=$ABI HEADER=$HEADER V=1
            if [ $? -ne 0 ]; then
                echo "ERROR: Can't compile CrystaX header <$HEADER> alone" 1>&2
                exit 1
            fi
            echo ""
            echo "===> OK: CrystaX header $HEADER for $PLATFORM/$ABI"
        done
    done
done

exit 0
