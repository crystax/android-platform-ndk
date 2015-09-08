#!/bin/bash

cd $(dirname $0) || exit 1

if [ -z "$NDK" ]; then
    NDK=$(cd ../../.. && pwd)
fi

source $NDK/build/tools/dev-defaults.sh || exit 1

if [ -z "$NDK_TOOLCHAIN_VERSION" ]; then
    TOOLCHAIN_VERSIONS="$DEFAULT_GCC_VERSION_LIST"
    for VERSION in $DEFAULT_LLVM_VERSION_LIST; do
        TOOLCHAIN_VERSIONS="$TOOLCHAIN_VERSIONS clang${VERSION}"
    done
else
    TOOLCHAIN_VERSIONS=$NDK_TOOLCHAIN_VERSION
fi

BOOST_VERSIONS=$(ls -1 $NDK/sources/boost/ 2>/dev/null)
if [ -z "$BOOST_VERSIONS" ]; then
    echo "*** ERROR: Can't find Boost libraries in $NDK/sources/boost" 1>&2
    exit 1
fi

run()
{
    echo "## COMMAND: $@"
    "$@"
}

cleanup()
{
    rm -Rf obj libs jni/Android.mk
}

for TV in $TOOLCHAIN_VERSIONS; do
    for VERSION in $BOOST_VERSIONS; do
        for LIBCXX in gnustl c++; do
            for LIBTYPE in shared static; do
                cleanup || exit 1

                {
                    echo 'LOCAL_PATH := $(call my-dir)'
                    echo ''
                    echo 'include $(CLEAR_VARS)'
                    echo 'LOCAL_MODULE := test-boost'$VERSION'-dependencies'
                    echo 'LOCAL_SRC_FILES := test.cpp'
                    echo 'LOCAL_STATIC_LIBRARIES := boost_regex_'$LIBTYPE
                    echo 'include $(BUILD_EXECUTABLE)'
                    echo ''
                    echo '$(call import-module,boost+icu/'$VERSION')'
                } | cat >jni/Android.mk || exit 1

                run $NDK/ndk-build -B "$@" APP_ABI=all APP_STL=${LIBCXX}_${LIBTYPE} NDK_TOOLCHAIN_VERSION=$TV V=1
                if [ $? -ne 0 ]; then
                    echo "*** ERROR: Can't build Boost dependencies test" 1>&2
                    exit 1
                fi
            done
        done
    done
done

cleanup
exit 0
