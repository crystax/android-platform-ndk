#!/bin/bash

if [ -z "$NDK" ]; then
    echo "ERROR: NDK is not defined" 1>&2
    exit 1
fi

if [ ! -d $NDK ]; then
    echo "ERROR: No such directory: $NDK" 1>&2
    exit 1
fi

find $NDK/tests -type d -a -name host -print | (
    files=""
    while read dir; do
        if [ -f $dir/DISABLED ]; then
            files="$files $dir/DISABLED"
        fi
    done
    if [ -n "$files" ]; then
        echo "ERROR: Several DISABLED files found:" 1>&2
        echo $files | tr ' ' '\n' 1>&2
        exit 1
    fi
    ) || exit 1

exit 0
