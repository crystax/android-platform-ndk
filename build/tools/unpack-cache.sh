#!/bin/bash
#
# Copyright (C) 2013, 2014 The Android Open Source Project
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
# This script is used to unpack cache content into NDK directory.
#
# The script can be used if you have complete cache and a clean
# repository or after git clean command.
#
# BEWARE!
#
#   This script will overwrite without remorse any existing files
#   with cache contents!
#
#

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

# no magic here
for archive in $NDK_CACHE_DIR/*.tar.bz2 ; do
    echo "Unpacking: $archive"
    unpack_archive "$archive" "$ANDROID_NDK_ROOT"
done
