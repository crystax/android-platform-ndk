#!/bin/bash

ls -1d $NDK/platforms/android-*/arch-*/usr/lib* | {
    while read dir; do
        echo $dir | grep -q "\<arch-p\>" && continue
        for lib in libm.so libcrystax.a libcrystax.so; do
            if [ -e $dir/$lib ]; then
                echo "ERROR: Found $lib in $dir (must not be there)" 1>&2
                exit 1
            fi
        done
    done
} || exit 1

for lib in libbz2.a libm.a libm_hard.a libc.a libpthread.a librt.a; do
    ls -1d $NDK/platforms/android-*/arch-*/usr/lib* | {
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
