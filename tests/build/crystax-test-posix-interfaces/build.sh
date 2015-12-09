#!/bin/bash

# Copyright (c) 2011-2015 CrystaX.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX.

cd $(dirname $0) || exit 1
MYDIR=$(pwd)

if [ -z "$NDK" ]; then
    NDK=$(cd ../../.. && pwd)
fi

if [ -z "$PLATFORMS" ]; then
    PLATFORMS=$(cd $NDK/platforms && ls -d android-* | sort)
else
    PLATFORMS=$(echo $PLATFORMS | sed 's/,/ /g')
fi

$MYDIR/bin/gen-all
if [ $? -ne 0 ]; then
    echo "ERROR: Code generation failed" 1>&2
    exit 1
fi

run()
{
    echo "## COMMAND: $@"
    "$@"
}

for PLATFORM in $PLATFORMS; do
    echo "===== Checking standard headers for $PLATFORM"
    APILEVEL=$(echo $PLATFORM | sed 's,^android-,,')
    if [ $APILEVEL -lt 9 ]; then
        ABIS="armeabi armeabi-v7a armeabi-v7a-hard"
    elif [ $APILEVEL -lt 21 ]; then
        ABIS="all32"
    else
        ABIS="all"
    fi

    run $NDK/ndk-build -C $MYDIR -B APP_PLATFORM=$PLATFORM APP_ABI="$ABIS"
    if [ $? -ne 0 ]; then
        echo "ERROR: Standard headers checks failed for $PLATFORM" 1>&2
        exit 1
    fi
    echo "===== OK: standard headers for $PLATFORM seems to be in good state"
done

exit 0
