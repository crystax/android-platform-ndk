#!/bin/bash

cd $(dirname $0) || exit 1
MYDIR=$(pwd)

if [ -z "$NDK" ]; then
    NDK=$(cd ../../.. && pwd)
fi

INCDIR=$NDK/sources/crystax/include

PLATFORMS=$(cd $NDK/platforms && ls -d android-*)

HEADERS=""
HEADERS="$HEADERS $(find $INCDIR -name '*.h' -print)"
HEADERS="$HEADERS $(find $INCDIR -name '*.hpp' -print)"

for HEADER in $HEADERS; do
    HEADER=${HEADER##$INCDIR/}
    for PLATFORM in $PLATFORMS; do
        echo "== Compiling CrystaX header $HEADER for $PLATFORM"
        $NDK/ndk-build -C $MYDIR -B APP_CFLAGS=-DHEADER=\"\<$HEADER\>\" APP_PLATFORM=$PLATFORM HEADER=$HEADER
        if [ $? -ne 0 ]; then
            echo "ERROR: Can't compile CrystaX header <$HEADER> alone" 1>&2
            exit 1
        fi
    done
done

exit 0
