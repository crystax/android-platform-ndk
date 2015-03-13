#!/bin/bash

if [ -z "$NDK" ]; then
    echo "ERROR: NDK is not defined" 1>&2
    exit 1
fi

if [ ! -d $NDK ]; then
    echo "ERROR: No such directory: $NDK" 1>&2
    exit 1
fi

files=$(find $NDK/tests -name BROKEN_BUILD -print)
if [ $? -ne 0 ]; then
    echo "ERROR: Can't search $NDK for BROKEN_BUILD files" 1>&2
    exit 1
fi

if [ -n "$files" ]; then
    echo "ERROR: Found several BROKEN_BUILD files in $NDK:" 1>&2
    echo $files | tr -s ' ' | tr ' ' '\n' 1>&2
    exit 1
fi

exit 0
