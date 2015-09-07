#!/bin/bash

if [ -z "$NDK" ]; then
    echo "*** ERROR: 'NDK' is not defined!" 1>&2
    exit 1
fi

run()
{
    echo "## COMMAND: $@"
    "$@"
}

for LIBCXX in gnustl c++; do
    for LIBTYPE in shared static; do
        run $NDK/ndk-build -B "$@" APP_ABI=all APP_STL=${LIBCXX}_${LIBTYPE} V=1 || exit 1
    done
done
