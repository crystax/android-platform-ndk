#!/bin/bash

if [ -z "$NDK" ]; then
    echo "ERROR: NDK is not defined" 1>&2
    exit 1
fi

$NDK/ndk-build "$@"
if [ $? -eq 0 ]; then
    echo "ERROR: Build succeed, but it should fail" 1>&2
    exit 1
fi

exit 0
