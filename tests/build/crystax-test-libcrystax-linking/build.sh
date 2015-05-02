#!/bin/bash

cd $(dirname $0) || exit 1

if [ -z "$NDK" ]; then
    NDK=$(cd ../../.. && pwd)
fi

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

run()
{
    echo "## COMMAND: $@"
    "$@"
}

cleanup()
{
    rm -Rf jni obj libs
}

EXENAME=test

for T in static shared; do
    cleanup || exit 1
    mkdir -p jni || exit 1

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo ''
        echo 'include $(CLEAR_VARS)'
        echo "LOCAL_MODULE := $EXENAME"
        echo 'LOCAL_SRC_FILES := main.c'
        echo 'include $(BUILD_EXECUTABLE)'
    } | cat >jni/Android.mk || exit 1

    {
        echo 'int main() { return 0; }'
    } | cat >jni/main.c || exit 1

    run $NDK/ndk-build -B "$@" APP_ABI=all APP_LIBCRYSTAX=$T V=1
    if [ $? -ne 0 ]; then
        echo "*** ERROR: Can't build $T libcrystax linking test" 1>&2
        exit 1
    fi

    for ABI in $(ls -1 libs/); do
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

        READELF=$NDK/toolchains/$TOOLCHAIN_NAME/prebuilt/$HOST_TAG/bin/${TOOLCHAIN_PREFIX}-readelf

        if [ ! -f "$READELF" ]; then
            echo "*** ERROR: Missing binary: $READELF" 1>&2
            exit 1
        fi

        $READELF -d libs/$ABI/$EXENAME | grep -q "\<NEEDED\>.*\<libcrystax\.so\>"
        DEPENDS=$?
        if [ "$T" = "static" ]; then
            if [ $DEPENDS -eq 0 ]; then
                echo "*** ERROR: libs/$ABI/$EXENAME depend on libcrystax.so even though it was built with APP_LIBCRYSTAX=$T" 1>&2
                exit 1
            fi
        elif [ "$T" = "shared" ]; then
            if [ $DEPENDS -ne 0 ]; then
                echo "*** ERROR: libs/$ABI/$EXENAME do not depend on libcrystax.so even though it was built with APP_LIBCRYSTAX=$T" 1>&2
                exit 1
            fi
        fi
    done
done

cleanup
exit 0
