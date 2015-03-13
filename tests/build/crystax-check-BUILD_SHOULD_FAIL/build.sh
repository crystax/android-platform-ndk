#!/bin/bash

if [ -z "$NDK" ]; then
    echo "ERROR: NDK is not defined" 1>&2
    exit 1
fi

if [ ! -d $NDK ]; then
    echo "ERROR: No such directory: $NDK" 1>&2
    exit 1
fi

files=$(find $NDK/tests -name BUILD_SHOULD_FAIL -print)
if [ $? -ne 0 ]; then
    echo "ERROR: Can't search $NDK for BUILD_SHOULD_FAIL files" 1>&2
    exit 1
fi

if [ -n "$files" ]; then
    echo "ERROR: Found several BUILD_SHOULD_FAIL files in $NDK:" 1>&2
    echo $files | tr -s ' ' | tr ' ' '\n' 1>&2
    exit 1
fi

exit 0
