#!/bin/bash
#
# Copyright (C) 2011, 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# package-platforms.sh
#

PROGDIR=$(dirname "$0")
. "$PROGDIR/prebuilt-common.sh"

NDK_DIR=$ANDROID_NDK_ROOT

OPTION_SAMPLES=
PACKAGE_DIR=

usage()
{
    echo "Package platform files from an Android NDK tree"
    echo ""
    echo "options:"
    echo ""
    echo "  --help                Print this message"
    echo "  --ndk-dir=<path>      Use platforms from this NDK directory [$NDK_DIR]"
    echo "  --samples             Also package samples"
    echo "  --package-dir=<path>  Package platforms archive in specific path."
    echo ""
}

for opt do
    optarg=`expr "x$opt" : 'x[^=]*=\(.*\)'`
    case "$opt" in
        --help|-h|-\?)
            usage
            exit 0
            ;;
        --ndk-dir=*)
            NDK_DIR=$optarg
            ;;
        --samples)
            OPTION_SAMPLES=yes
            ;;
        --package-dir=*)
            PACKAGE_DIR=$optarg
            ;;
        *)
            echo "*** ERROR: unknown option '$opt'" 1>&2
            usage 1>&2
            exit 1
    esac
done

if [ -z "$PACKAGE_DIR" ]; then
    echo "*** ERROR: Package directory was not specified!" 1>&2
    usage 1>&2
    exit 1
fi

mkdir -p "$PACKAGE_DIR"
fail_panic "Could not create package directory: $PACKAGE_DIR"
ARCHIVE=platforms.tar.xz
dump "Packaging $ARCHIVE"
pack_archive "$PACKAGE_DIR/$ARCHIVE" "$NDK_DIR" "platforms"
fail_panic "Could not package platforms"
cache_package "$PACKAGE_DIR" "$ARCHIVE"
if [ "$OPTION_SAMPLES" ]; then
    ARCHIVE=samples.tar.xz
    dump "Packaging $ARCHIVE"
    pack_archive "$PACKAGE_DIR/$ARCHIVE" "$NDK_DIR" "samples"
    fail_panic "Could not package samples"
    cache_package "$PACKAGE_DIR" "$ARCHIVE"
fi

log "Done !"
