# Copyright (C) 2009 The Android Open Source Project
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
# A nawk/gawk script used to extract the application's platform name from
# its project.properties file. It is called from build/core/add-application.mk
#

# we look for a line that looks like one of:
#    arm-linux-androideabi-<version>
#    x86-<version>
#
# <version> is a sequence of digits separated by dot (e.g. 4.4.3 or 4.6.3)
#
BEGIN {
    version_regex="[0-9]+\\.[0-9]+\\.[0-9]+$"
    VERSION=unknown
}

/^[ \t]*TOOLCHAIN_NAME[ \t]*:=.*$/ {
    if (match($0,version_regex)) {
        VERSION=substr($0,RSTART,RLENGTH)
    }
}

END {
    printf("%s", VERSION)
}
