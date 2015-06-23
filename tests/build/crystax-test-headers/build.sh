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

ext_for_lang()
{
    local lang=$1
    if [ -z "$lang" ]; then
        echo "ERROR: You should pass languages to call of ext_for_lang" 1>&2
        exit 1
    fi

    case $lang in
        c)      echo "c"   ;;
        c++)    echo "cpp" ;;
        objc)   echo "m"   ;;
        objc++) echo "mm"  ;;
        *)
            echo "ERROR: Unknown language: '$lang'" 1>&2
            exit 1
    esac
}

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
        echo all32
    fi
}

run()
{
    echo "## COMMAND: $@"
    "$@"
}

rm -Rf jni obj libs
mkdir -p jni || exit 1

{
    echo 'LOCAL_PATH := $(call my-dir)'
    echo ''
    echo 'include $(CLEAR_VARS)'
    echo 'LOCAL_MODULE := crystax-test-headers'
    echo 'LOCAL_SRC_FILES := main.c'
} | cat >jni/Android.mk || exit 1

{
    echo 'int main() { return 0; }'
} | cat >jni/main.c || exit 1

num=0

for HEADER in $HEADERS; do
    HEADER=${HEADER##$INCDIR/}

    # Skip internal headers
    [[ ${HEADER} == "_ctype.h"    ]] && continue
    [[ ${HEADER} == "ieeefp.h"    ]] && continue
    [[ ${HEADER##crystax/arm64/}    != $HEADER ]] && continue
    [[ ${HEADER##crystax/details}   != $HEADER ]] && continue
    [[ ${HEADER##crystax/dlmalloc/} != $HEADER ]] && continue
    [[ ${HEADER##crystax/freebsd/}  != $HEADER ]] && continue
    [[ ${HEADER##crystax/mips64/}   != $HEADER ]] && continue
    [[ ${HEADER##crystax/sys/}      != $HEADER ]] && continue
    [[ ${HEADER##linux/}            != $HEADER ]] && continue
    [[ ${HEADER##machine/}          != $HEADER ]] && continue
    [[ ${HEADER##sys/_}             != $HEADER ]] && continue
    [[ ${HEADER##xlocale/}          != $HEADER ]] && continue

    let num+=1

    for LANG in c c++ objc objc++; do
        fname=test${num}-${LANG}.$(ext_for_lang $LANG)

        {
            echo "#include <${HEADER}>"
            echo ''
            echo "#if !defined(__LIBCRYSTAX) || __LIBCRYSTAX != 1"
            echo "#error \"__LIBCRYSTAX macro is not defined\""
            echo "#endif"
        } | cat >jni/$fname || exit 1

        SKIP=no
        case $LANG in
            c|objc)
                echo $HEADER | grep -q '\.hpp$'
                if [ $? -eq 0 ]; then
                    SKIP=yes
                fi
                ;;
        esac
        if [ "$SKIP" != "yes" ]; then
            echo "LOCAL_SRC_FILES += $fname" >>jni/Android.mk || exit 1
        fi
    done

done

{
    echo 'include $(BUILD_EXECUTABLE)'
} | cat >>jni/Android.mk || exit 1

for PLATFORM in $PLATFORMS; do
    echo ""
    echo "================================================================="
    echo "===> Testing CrystaX headers for $PLATFORM <==="
    echo "================================================================="
    echo ""

    #echo "APP_ABI := $(abis_for_platform $PLATFORM)" >jni/Application.mk || exit 1

    run $NDK/ndk-build -C $MYDIR -B "$@" APP_PLATFORM=$PLATFORM APP_ABI=$(abis_for_platform $PLATFORM) V=1
    if [ $? -ne 0 ]; then
        echo "ERROR: Can't compile CrystaX headers for $PLATFORM alone" 1>&2
        exit 1
    fi
    echo ""
    echo "===> OK: CrystaX headers for $PLATFORM"
done

rm -Rf jni obj libs

exit 0
