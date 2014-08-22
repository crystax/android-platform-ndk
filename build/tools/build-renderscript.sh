#!/bin/bash
#
# Copyright (C) 2014 The Android Open Source Project
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
# This script is used to rebuild the host 'ndk-stack' tool from the
# sources under sources/host-tools/ndk-stack.
#
# Note: The tool is installed under prebuilt/$HOST_TAG/bin/ndk-stack
#       by default.
#
PROGDIR=$(dirname $0)
. $PROGDIR/prebuilt-common.sh

PROGRAM_PARAMETERS=""
PROGRAM_DESCRIPTION=\
"This script is used to copy RenderScript prebuilts into specified dir."

# the list of supported host development systems
SYSTEMS=$DEFAULT_SYSTEMS
register_var_option "--systems=<list>" SYSTEMS "Specify host systems"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Archive binary into specific directory"

extract_parameters "$@"

SRCDIR=$ANDROID_NDK_ROOT/../../platform/prebuilts/rs

if [ -z "$PACKAGE_DIR" ]; then
    fail_panic "PACKAGE_DIR parameter is required"
else
    cp "$SRCDIR/renderscript.tar.bz2" "$PACKAGE_DIR/"
    fail_panic "Failed to copy $SRCDIR/renderscript.tar.bz2 to $PACKAGE_DIR/"
    for SYSTEM in $SYSTEMS; do
        cp "$SRCDIR/renderscript-$SYSTEM.tar.bz2" "$PACKAGE_DIR/"
        fail_panic "Failed to copy $SRCDIR/renderscript.tar.bz2 to $PACKAGE_DIR/"
    done
fi

log "Done!"
exit 0
