#!/bin/bash

fail_panic ()
{
    if [ $? != 0 ] ; then
        echo "ERROR: $@" 1>&2
        exit 1
    fi
}

for opt do
    optarg=`expr "x$opt" : 'x[^=]*=\(.*\)'`
    case "$opt" in
    APP_ABI=*)
        APP_ABI=$optarg
        ;;
    esac
done

if [ -z "$APP_ABI" -o "$APP_ABI" = "all" ]; then
    APP_ABI="armeabi-v7a,armeabi-v7a-hard,x86,mips,arm64-v8a,x86_64,mips64"
fi

APP_ABI=`echo $APP_ABI | tr ',' ' '`
for ABI in $APP_ABI; do
    echo "=== $ABI"
    rm -Rf libs obj
    $NDK/ndk-build -B APP_ABI=$ABI APP_CFLAGS="-g -save-temps"
    fail_panic "can't compile for APP_ABI=$ABI"
done

rm -rf libs obj
