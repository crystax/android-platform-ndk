#!/bin/bash

libm_so_count=$(find $NDK/platforms/android-*/arch-*/usr/lib -name 'libm.so' -print 2>/dev/null)
if [ -n "$libm_so_count" ]; then
    echo "ERROR: Found libm.so in platforms sysroot (must not be there)" 1>&2
    echo "$libm_so_count" 1>&2
    exit 1
fi

for lib in libbz2.a libm.a libm_hard.a; do
    ls -1d $NDK/platforms/android-*/arch-*/usr/lib | {
        while read dir; do
            echo $dir | grep -q "\<arch-p\>" && continue
            if [ ! -e $dir/$lib ]; then
                echo "ERROR: No $lib found in $dir" 1>&2
                exit 1
            fi
        done
    } || exit 1

    find $NDK/platforms -name "$lib" -print | {
        while read f; do
            size=$(du -b $f | awk '{print $1}')
            if [ $size -ne 8 ]; then
                echo "ERROR: File $f is not empty stub" 1>&2
                exit 1
            fi
        done
    } || exit 1
done

echo "OK"
