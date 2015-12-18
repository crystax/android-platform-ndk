#!/bin/bash

if [ -z "$NDK" ]; then
    echo "ERROR: NDK is not defined" 1>&2
    exit 1
fi

if [ ! -d $NDK ]; then
    echo "ERROR: No such directory: $NDK" 1>&2
    exit 1
fi

fnames="        \
    _hashlib.so \
    _ssl.so     \
    "

for fname in $fnames; do
    files=$(find $NDK/prebuilt -name $fname -print)
    if [ $? -ne 0 ]; then
        echo "ERROR: Can't search $NDK for '$fname'" 1>&2
        exit 1
    fi

    if [ -n "$files" ]; then
        echo "ERROR: Found several '$fname' files in $NDK:" 1>&2
        echo $files | tr -s ' ' | tr ' ' '\n' 1>&2
        exit 1
    fi
done

exit 0
