#!/bin/bash
#
# Copyright (c) 2011-2016 CrystaX.
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


NDK_BUILDTOOLS_PATH="$(dirname $0)"
. "$NDK_BUILDTOOLS_PATH/prebuilt-common.sh"
. "$NDK_BUILDTOOLS_PATH/common-build-host-funcs.sh"

PROGRAM_PARAMETERS=""
PROGRAM_DESCRIPTION="\
This program is used to rebuild Python client programs from sources for the systems
specified in command line: --systems=<tag-1>[,<tag-2>, ...>]
Each <tag> value can be one of the following:
   linux-x86
   linux-x86_64
   windows
   windows-x86_64
   darwin-x86
   darwin-x86_64
"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Select NDK install directory."

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Package prebuilt tarballs into directory."

BUILD_DIR=
register_var_option "--build-dir=<path>" BUILD_DIR "Build Python into directory"

bh_register_options
register_try64_option
register_canadian_option
register_jobs_option

extract_parameters "$@"

# setup var BH_BUILD_DIR
bh_setup_build_dir $BUILD_DIR

CMAKE_MIN_VERSION="2.8.3"

PYTHON_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$PYTHON_SRCDIR" ]; then
    panic "Please provide the path to the python source tree. See --help"
fi

if [ ! -d "$PYTHON_SRCDIR" ]; then
    panic "No such directory: '$PYTHON_SRCDIR'"
fi

PYTHON_SRCDIR=$(cd $PYTHON_SRCDIR && pwd)

PYTHON_MAJOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MAJOR_VERSION[ \t]*\([0-9]*\).*/\1/p')
if [ -z "$PYTHON_MAJOR_VERSION" ]; then
    panic "Can't detect python major version."
fi

PYTHON_MINOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MINOR_VERSION[ \t]*\([0-9]*\).*/\1/p')
if [ -z "$PYTHON_MINOR_VERSION" ]; then
    panic "Can't detect python minor version."
fi

PYTHON_ABI="$PYTHON_MAJOR_VERSION"'.'"$PYTHON_MINOR_VERSION"

PYTHON_BUILD_UTILS_DIR=$(cd $(dirname $0)/build-python && pwd)
PYTHON_BUILD_UTILS_DIR_HOST="$PYTHON_BUILD_UTILS_DIR/host"
if [ ! -d "$PYTHON_BUILD_UTILS_DIR_HOST" ]; then
    panic "No such directory: '$PYTHON_BUILD_UTILS_DIR_HOST'"
fi

PY_CMAKE_TEMPLATE_FILE="$PYTHON_BUILD_UTILS_DIR_HOST/CMakeLists.txt.$PYTHON_ABI"
if [ ! -f "$PY_CMAKE_TEMPLATE_FILE" ]; then
    panic "Build of host python $PYTHON_ABI is not supported, no such file: $PY_CMAKE_TEMPLATE_FILE"
fi

PY_C_CONFIG_FILE="$PYTHON_BUILD_UTILS_DIR_HOST/config.c.$PYTHON_ABI"
if [ ! -f "$PY_C_CONFIG_FILE" ]; then
    panic "Build of host python $PYTHON_ABI is not supported, no such file: $PY_C_CONFIG_FILE"
fi

PY_C_INTERPRETER_STUB_FILE="$PYTHON_BUILD_UTILS_DIR_HOST/host-interpreter-stub.c.$PYTHON_ABI"
if [ ! -f "$PY_C_INTERPRETER_STUB_FILE" ]; then
    panic "Build of host python $PYTHON_ABI is not supported, no such file: $PY_C_INTERPRETER_STUB_FILE"
fi

determine_systems ()
{
    local IN_SYSTEMS="$1"
    local OUT_SYSTEMS

    for SYSTEM in $IN_SYSTEMS; do
        if [ "$TRY64" = "yes" ]; then
            case $SYSTEM in
                darwin-x86|linux-x86|windows-x86)
                    SYSTEM=${SYSTEM%%x86}x86_64
                    ;;
                windows)
                    SYSTEM=windows-x86_64
                    ;;
            esac
        else
            case $SYSTEM in
                windows-x86)
                    SYSTEM=windows
                    ;;
            esac
        fi
        OUT_SYSTEMS="$OUT_SYSTEMS $SYSTEM"
    done
    echo $OUT_SYSTEMS
}

BH_HOST_SYSTEMS=$(commas_to_spaces $BH_HOST_SYSTEMS)
if [ "$MINGW" = "yes" ]; then
    BH_HOST_SYSTEMS="windows"
    log "Auto-config: --systems=windows"
    find_mingw_toolchain
fi
if [ "$DARWIN" = "yes" ]; then
    BH_HOST_SYSTEMS="darwin-x86"
    log "Auto-config: --systems=darwin-x86"
fi
BH_HOST_SYSTEMS=$(determine_systems "$BH_HOST_SYSTEMS")
for SYSTEM in $BH_HOST_SYSTEMS; do
    bh_setup_build_for_host $SYSTEM
done

# $1: host system tag
build_python_stub ()
{
    local BUILDDIR_PYSTUB="$BH_BUILD_DIR/pystub"
    local BUILDDIR_PYSTUB_CONFIG="$BUILDDIR_PYSTUB/config"
    local BUILDDIR_PYSTUB_CORE="$BUILDDIR_PYSTUB/core"
    local BUILDDIR_PYSTUB_INTERPRETER_0="$BUILDDIR_PYSTUB/interpreter-0"
    local BUILDDIR_PYSTUB_INTERPRETER_1="$BUILDDIR_PYSTUB/interpreter-1"
    local BUILDDIR_PYSTUB_BIN="$BUILDDIR_PYSTUB/bin"
    run mkdir -p $BUILDDIR_PYSTUB_CONFIG
    fail_panic "Can't create directory: $BUILDDIR_PYSTUB_CONFIG"
    local PYSTUB_CONFIGURE_WRAPPER="$BUILDDIR_PYSTUB_CONFIG/configure.sh"
    {
        echo "#!/bin/bash -e"
        echo ''
        echo 'cd $(dirname $0)'
        echo ''
        if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
            echo "exec $PYTHON_SRCDIR/configure \\"
            echo "    --prefix=$BUILDDIR_PYSTUB_CONFIG/install \\"
            echo "    --enable-shared \\"
            echo "    --with-threads \\"
            echo "    --enable-ipv6 \\"
            echo "    --enable-unicode=ucs4 \\"
            echo "    --without-ensurepip"
        else
            echo "exec $PYTHON_SRCDIR/configure \\"
            echo "    --prefix=$BUILDDIR_PYSTUB_CONFIG/install \\"
            echo "    --enable-shared \\"
            echo "    --with-threads \\"
            echo "    --enable-ipv6 \\"
            echo "    --with-computed-gotos \\"
            echo "    --without-ensurepip"
        fi
    } >$PYSTUB_CONFIGURE_WRAPPER
    fail_panic "Can't create configure wrapper: $PYSTUB_CONFIGURE_WRAPPER"
    run chmod +x $PYSTUB_CONFIGURE_WRAPPER
    fail_panic "Can't chmod +x configure wrapper: $PYSTUB_CONFIGURE_WRAPPER"
    log "configure python-$PYTHON_ABI for $BH_BUILD_TAG ..."
    run $PYSTUB_CONFIGURE_WRAPPER
    fail_panic "Can't configure python-$PYTHON_ABI for $BH_BUILD_TAG"

    run mkdir -p $BUILDDIR_PYSTUB_CORE
    fail_panic "Can't create directory: $BUILDDIR_PYSTUB_CORE"
    local PYSTUB_CORE_CMAKE_DESCRIPTION="$BUILDDIR_PYSTUB_CORE/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo "set(MY_PYTHON_SRC_ROOT \"$PYTHON_SRCDIR\")"
        cat $PY_CMAKE_TEMPLATE_FILE
    } >$PYSTUB_CORE_CMAKE_DESCRIPTION
    fail_panic "Can't generate $PYSTUB_CORE_CMAKE_DESCRIPTION"
    local PYSTUB_CORE_BUILD_WRAPPER="$BUILDDIR_PYSTUB_CORE/build.sh"
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo 'cmake $DIR_HERE'
        echo 'make VERBOSE=1'
    } >$PYSTUB_CORE_BUILD_WRAPPER
    fail_panic "Can't create build wrapper: $PYSTUB_CORE_BUILD_WRAPPER"
    run chmod +x $PYSTUB_CORE_BUILD_WRAPPER
    fail_panic "Can't chmod +x build wrapper: $PYSTUB_CORE_BUILD_WRAPPER"
    run cp -p -T $PY_C_CONFIG_FILE "$BUILDDIR_PYSTUB_CORE/config.c" && \
        cp -p -t "$BUILDDIR_PYSTUB_CORE" "$BUILDDIR_PYSTUB_CONFIG/pyconfig.h"
    fail_panic "Can't copy config.c pyconfig.h to $BUILDDIR_PYSTUB_CORE"

    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PY_C_GETPATH="$PYTHON_BUILD_UTILS_DIR/getpath.c.$PYTHON_ABI"
        run cp -p -T $PY_C_GETPATH "$BUILDDIR_PYSTUB_CORE/getpath.c"
        fail_panic "Can't copy $PY_C_GETPATH to $BUILDDIR_PYSTUB_CORE"
    fi
    log "build python-$PYTHON_ABI core for $BH_BUILD_TAG ..."
    run $PYSTUB_CORE_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI core for $BH_BUILD_TAG"
    run mkdir -p $BUILDDIR_PYSTUB_BIN
    fail_panic "Can't create directory: $BUILDDIR_PYSTUB_BIN"
    local PY_CORE_FNAME
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        PY_CORE_FNAME="libpython${PYTHON_ABI}.so"
    else
        PY_CORE_FNAME="libpython${PYTHON_ABI}m.so"
    fi
    run cp -p -t "$BUILDDIR_PYSTUB_BIN" "$BUILDDIR_PYSTUB_CORE/build/$PY_CORE_FNAME"
    fail_panic "Can't copy '$PY_CORE_FNAME' from $BUILDDIR_PYSTUB_CORE/build to '$BUILDDIR_PYSTUB_BIN'"

    run mkdir -p $BUILDDIR_PYSTUB_INTERPRETER_0
    fail_panic "Can't create directory: $BUILDDIR_PYSTUB_INTERPRETER_0"
    local PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_0="$BUILDDIR_PYSTUB_INTERPRETER_0/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo 'set(CMAKE_BUILD_TYPE RELEASE)'
        echo 'set(CMAKE_EXE_LINKER_FLAGS "-ldl")'
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
        echo "set(CMAKE_C_FLAGS \"-DPYTHON_STDLIB_PATH=\\\\\\\"$PYTHON_SRCDIR/Lib\\\\\\\"\")"
    } >$PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_0
    fail_panic "Can't generate $PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_0"
    local PYSTUB_INTERPRETER_0_BUILD_WRAPPER="$BUILDDIR_PYSTUB_INTERPRETER_0/build.sh"
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo 'cmake $DIR_HERE'
        echo 'make VERBOSE=1'
    } >$PYSTUB_INTERPRETER_0_BUILD_WRAPPER
    fail_panic "Can't create build wrapper: $PYSTUB_INTERPRETER_0_BUILD_WRAPPER"
    run chmod +x $PYSTUB_INTERPRETER_0_BUILD_WRAPPER
    fail_panic "Can't chmod +x build wrapper: $PYSTUB_INTERPRETER_0_BUILD_WRAPPER"
    run cp -p -T $PY_C_INTERPRETER_STUB_FILE "$BUILDDIR_PYSTUB_INTERPRETER_0/interpreter.c"
    fail_panic "Can't copy $PY_C_INTERPRETER_STUB_FILE to $BUILDDIR_PYSTUB_INTERPRETER_0"
    log "build python-$PYTHON_ABI interpreter(0) stub for $BH_BUILD_TAG ..."
    run $PYSTUB_INTERPRETER_0_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI interpreter(0) stub for $BH_BUILD_TAG"
    run cp -p -t "$BUILDDIR_PYSTUB_BIN" "$BUILDDIR_PYSTUB_INTERPRETER_0/build/python"
    fail_panic "Can't copy python from $BUILDDIR_PYSTUB_INTERPRETER_0/build to $BUILDDIR_PYSTUB_BIN"

    log "build python-$PYTHON_ABI stdlib for $BH_BUILD_TAG ..."
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        run $BUILDDIR_PYSTUB_BIN/python -B -S $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --py2 --pysrc-root $PYTHON_SRCDIR --output-zip "$BUILDDIR_PYSTUB_BIN/stdlib.zip"
        fail_panic "Can't install python $PYTHON_ABI stdlib"
    else
        run $BUILDDIR_PYSTUB_BIN/python -B -S $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --pysrc-root $PYTHON_SRCDIR --output-zip "$BUILDDIR_PYSTUB_BIN/stdlib.zip"
        fail_panic "Can't generate python $PYTHON_ABI stdlib"
    fi

    run rm -f "$BUILDDIR_PYSTUB_BIN/python"
    fail_panic "Can't remove $BUILDDIR_PYSTUB_BIN/python"
    run mkdir -p $BUILDDIR_PYSTUB_INTERPRETER_1
    fail_panic "Can't create directory: $BUILDDIR_PYSTUB_INTERPRETER_1"
    local PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_1="$BUILDDIR_PYSTUB_INTERPRETER_1/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo 'set(CMAKE_BUILD_TYPE RELEASE)'
        echo 'set(CMAKE_EXE_LINKER_FLAGS "-ldl")'
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
        echo "set(CMAKE_C_FLAGS \"-DPYTHON_STDLIB_PATH=\\\\\\\"stdlib.zip\\\\\\\"\")"
    } >$PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_1
    fail_panic "Can't generate $PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_1"
    local PYSTUB_INTERPRETER_1_BUILD_WRAPPER="$BUILDDIR_PYSTUB_INTERPRETER_1/build.sh"
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo 'cmake $DIR_HERE'
        echo 'make VERBOSE=1'
    } >$PYSTUB_INTERPRETER_1_BUILD_WRAPPER
    fail_panic "Can't create build wrapper: $PYSTUB_INTERPRETER_1_BUILD_WRAPPER"
    run chmod +x $PYSTUB_INTERPRETER_1_BUILD_WRAPPER
    fail_panic "Can't chmod +x build wrapper: $PYSTUB_INTERPRETER_1_BUILD_WRAPPER"
    run cp -p -T $PY_C_INTERPRETER_STUB_FILE "$BUILDDIR_PYSTUB_INTERPRETER_1/interpreter.c"
    fail_panic "Can't copy $PY_C_INTERPRETER_STUB_FILE to $BUILDDIR_PYSTUB_INTERPRETER_1"
    log "build python-$PYTHON_ABI interpreter(1) stub for $BH_BUILD_TAG ..."
    run $PYSTUB_INTERPRETER_1_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI interpreter(1) stub for $BH_BUILD_TAG"
    run cp -p -t "$BUILDDIR_PYSTUB_BIN" "$BUILDDIR_PYSTUB_INTERPRETER_1/build/python"
    fail_panic "Can't copy python from $BUILDDIR_PYSTUB_INTERPRETER_1/build to $BUILDDIR_PYSTUB_BIN"
}

# $1: host system tag
build_host_python ()
{
# Step 1: build python stub once
    bh_stamps_do "build-vendor-python$PYTHON_ABI-stub" build_python_stub
    local PYTHON_FOR_BUILD="$BH_BUILD_DIR/pystub/bin/python"

# Step 2: prepare cmake toolchain wrappers for host
    local OBJ_DIR="$BH_BUILD_DIR/obj-$1"
    run mkdir -p $OBJ_DIR
    fail_panic "Can't create directory: $OBJ_DIR"
    local OUTPUT_DIR="$(python_build_install_dir $1)/opt/python$PYTHON_ABI"
    run mkdir -p $OUTPUT_DIR
    fail_panic "Can't create directory: $OUTPUT_DIR"

    local TOOLCHAIN_WRAPPER_PREFIX
    local CMAKE_CROSS_SYSTEM_NAME
    case $1 in
        windows-x86_64)
            TOOLCHAIN_WRAPPER_PREFIX="x86_64-w64-mingw32"
            CMAKE_CROSS_SYSTEM_NAME="Windows"
            ;;
        windows*)
            TOOLCHAIN_WRAPPER_PREFIX="i686-w64-mingw32msvc"
            CMAKE_CROSS_SYSTEM_NAME="Windows"
            ;;
        linux-x86_64)
            TOOLCHAIN_WRAPPER_PREFIX="x86_64-linux-gnu"
            CMAKE_CROSS_SYSTEM_NAME="Linux"
            ;;
        linux-x86)
            TOOLCHAIN_WRAPPER_PREFIX="i686-linux-gnu"
            CMAKE_CROSS_SYSTEM_NAME="Linux"
            ;;
        darwin-x86_64)
            TOOLCHAIN_WRAPPER_PREFIX="x86_64-apple-darwin"
            CMAKE_CROSS_SYSTEM_NAME="Darwin"
            ;;
        darwin-x86)
            TOOLCHAIN_WRAPPER_PREFIX="i686-apple-darwin"
            CMAKE_CROSS_SYSTEM_NAME="Darwin"
            ;;
        *)
            panic "Unknown platform: $1"
            ;;
    esac
    local CMAKE_TOOLCHAIN_WRAPPER="$OBJ_DIR/toolchain.cmake"
    {
        echo "set(CMAKE_SYSTEM_NAME $CMAKE_CROSS_SYSTEM_NAME)"
        echo "set(CMAKE_C_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc)"
        echo "set(CMAKE_CXX_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-g++)"
        case $1 in
            windows*)
                echo "set(CMAKE_RC_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-windres)"
            ;;
        esac
    } >$CMAKE_TOOLCHAIN_WRAPPER
    fail_panic "Can't create cmake-toolchain wrapper: $CMAKE_TOOLCHAIN_WRAPPER"

# Step 3: build python core for host
    local BUILDDIR_CORE="$OBJ_DIR/core"
    run mkdir -p $BUILDDIR_CORE
    fail_panic "Can't create directory: $BUILDDIR_CORE"
    local CORE_CMAKE_DESCRIPTION="$BUILDDIR_CORE/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo "set(MY_PYTHON_SRC_ROOT \"$PYTHON_SRCDIR\")"
        cat $PY_CMAKE_TEMPLATE_FILE
    } >$CORE_CMAKE_DESCRIPTION
    fail_panic "Can't generate $CORE_CMAKE_DESCRIPTION"
    local CORE_BUILD_WRAPPER="$BUILDDIR_CORE/build.sh"
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo "cmake -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_WRAPPER cmake \$DIR_HERE"
        echo 'make VERBOSE=1'
    } >$CORE_BUILD_WRAPPER
    fail_panic "Can't create build wrapper: $CORE_BUILD_WRAPPER"
    run chmod +x $CORE_BUILD_WRAPPER
    fail_panic "Can't chmod +x build wrapper: $CORE_BUILD_WRAPPER"
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PY_C_GETPATH="$PYTHON_BUILD_UTILS_DIR/getpath.c.$PYTHON_ABI"
        run cp -p -T $PY_C_GETPATH "$BUILDDIR_CORE/getpath.c"
        fail_panic "Can't copy $PY_C_GETPATH to $BUILDDIR_CORE"
    fi
    run cp -p -T $PY_C_CONFIG_FILE "$BUILDDIR_CORE/config.c"
    fail_panic "Can't copy config.c to $BUILDDIR_CORE"
    case $1 in
        windows*)
            run cp -p -t "$BUILDDIR_CORE" \
                "$PYTHON_SRCDIR/PC/pyconfig.h" \
                "$PYTHON_SRCDIR/PC/errmap.h" \
                "$PYTHON_SRCDIR/Python/importdl.h" \
                "$PYTHON_SRCDIR/Python/dynload_win.c" \
                "$PYTHON_SRCDIR/Modules/posixmodule.c"
            fail_panic "Can't copy pyconfig.h errmap.h importdl.h dynload_win.c posixmodule.c to $BUILDDIR_CORE"
            if [ "$PYTHON_MAJOR_VERSION" == "3" ]; then
                run cp -p -t "$BUILDDIR_CORE" "$PYTHON_SRCDIR/PC/getpathp.c"
                fail_panic "Can't copy getpathp.c to $BUILDDIR_CORE"
            fi
            # fix CamelCase inclusions for windows.h and mstcpip.h
            local MINGW_ROOT=$(cd "$MINGW_PATH/.." && pwd)
            run find $MINGW_ROOT -name "windows.h" -exec ln -s {} "$BUILDDIR_CORE/Windows.h" \;
            fail_panic "Can't create symlink for Windows.h"
            run find $MINGW_ROOT -name "mstcpip.h" -exec ln -s {} "$BUILDDIR_CORE/MSTcpIP.h" \;
            fail_panic "Can't create symlink for mstcpip.h"
            run patch "$BUILDDIR_CORE/posixmodule.c" < "$PYTHON_BUILD_UTILS_DIR_HOST/posixmodule.c.$PYTHON_ABI.mingw.patch"
            fail_panic "Can't patch posixmodule.c"
            run patch "$BUILDDIR_CORE/dynload_win.c" < "$PYTHON_BUILD_UTILS_DIR_HOST/dynload_win.c.$PYTHON_ABI.mingw.patch"
            fail_panic "Can't patch dynload_win.c"
            if [ "$PYTHON_MAJOR_VERSION" == "3" ]; then
                run patch "$BUILDDIR_CORE/getpathp.c" < "$PYTHON_BUILD_UTILS_DIR_HOST/getpathp.c.$PYTHON_ABI.mingw.patch"
                fail_panic "Can't patch getpathp.c"
            fi
            ;;

        darwin*)
            run cp -p -t "$BUILDDIR_CORE" "$BH_BUILD_DIR/pystub/config/pyconfig.h"
            fail_panic "Can't copy pyconfig.h to $BUILDDIR_CORE"
            ;;

        linux*)
            local BUILDDIR_CONFIG="$OBJ_DIR/config"
            run mkdir -p $BUILDDIR_CONFIG
            fail_panic "Can't create directory: $BUILDDIR_CONFIG"
            local CONFIG_SITE="$BUILDDIR_CONFIG/config.site"
            {
                echo 'ac_cv_file__dev_ptmx=yes'
                echo 'ac_cv_file__dev_ptc=no'
            } >$CONFIG_SITE
            fail_panic "Can't create config.site wrapper: '$CONFIG_SITE'"

            local CONFIGURE_WRAPPER=$BUILDDIR_CONFIG/configure.sh
            {
                echo "#!/bin/bash -e"
                echo ''
                echo "export CC=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc\""
                echo "export CPP=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc -E\""
                echo "export AR=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-ar\""
                echo "export RANLIB=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-runlib\""
                echo "export READELF=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-readelf\""
                echo "export PYTHON_FOR_BUILD=\"$PYTHON_FOR_BUILD\""
                echo "export CONFIG_SITE=\"$CONFIG_SITE\""
                echo ''
                echo 'cd $(dirname $0)'
                echo ''
                if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
                    echo "exec $PYTHON_SRCDIR/configure \\"
                    echo "    --host=$BH_HOST_CONFIG \\"
                    echo "    --build=$BH_BUILD_CONFIG \\"
                    echo "    --prefix=$BUILDDIR_CONFIG/install \\"
                    echo "    --enable-shared \\"
                    echo "    --with-threads \\"
                    echo "    --enable-ipv6 \\"
                    echo "    --enable-unicode=ucs4 \\"
                    echo "    --without-ensurepip"
                else
                    echo "exec $PYTHON_SRCDIR/configure \\"
                    echo "    --host=$BH_HOST_CONFIG \\"
                    echo "    --build=$BH_BUILD_CONFIG \\"
                    echo "    --prefix=$BUILDDIR_CONFIG/install \\"
                    echo "    --enable-shared \\"
                    echo "    --with-threads \\"
                    echo "    --enable-ipv6 \\"
                    echo "    --with-computed-gotos \\"
                    echo "    --without-ensurepip"
                fi
            } >$CONFIGURE_WRAPPER
            fail_panic "Can't create configure wrapper: '$CONFIGURE_WRAPPER'"
            run chmod +x $CONFIGURE_WRAPPER
            fail_panic "Can't chmod +x configure wrapper: '$CONFIGURE_WRAPPER'"
            log "configure python-$PYTHON_ABI for $1 ..."
            run $CONFIGURE_WRAPPER
            fail_panic "Can't configure python-$PYTHON_ABI for $1"
            run cp -p -t "$BUILDDIR_CORE" "$BUILDDIR_CONFIG/pyconfig.h"
            fail_panic "Can't copy pyconfig.h to $BUILDDIR_CORE"
            ;;
        *)
            panic "Unknown platform: $1"
            ;;
    esac

    log "build python-$PYTHON_ABI core for $1 ..."
    run $CORE_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI core for $1"

    local PY_CORE_FNAME
    case $1 in
        windows*)
            PY_CORE_FNAME="python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.dll"
            ;;
        *)
        if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
            PY_CORE_FNAME="libpython${PYTHON_ABI}.so"
        else
            PY_CORE_FNAME="libpython${PYTHON_ABI}m.so"
        fi
        ;;
    esac
    run cp -p -t "$OUTPUT_DIR" "$BUILDDIR_CORE/build/$PY_CORE_FNAME"
    fail_panic "Can't copy '$PY_CORE_FNAME' to '$OUTPUT_DIR'"

# Step 4: build python interpreter for host
    local BUILDDIR_INTERPRETER="$OBJ_DIR/interpreter"

    run mkdir -p $BUILDDIR_INTERPRETER
    fail_panic "Can't create directory: $BUILDDIR_INTERPRETER"
    local INTERPRETER_CMAKE_DESCRIPTION="$BUILDDIR_INTERPRETER/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo 'set(CMAKE_BUILD_TYPE RELEASE)'
        case $1 in
            windows*)
                if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
                    echo 'set(CMAKE_EXE_LINKER_FLAGS "-municode")'
                fi
                ;;
            *)
                echo 'set(CMAKE_EXE_LINKER_FLAGS "-ldl")'
                ;;
        esac
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
    } >$INTERPRETER_CMAKE_DESCRIPTION
    fail_panic "Can't generate $INTERPRETER_CMAKE_DESCRIPTION"
    local INTERPRETER_BUILD_WRAPPER="$BUILDDIR_INTERPRETER/build.sh"
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo "cmake -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_WRAPPER cmake \$DIR_HERE"
        echo 'make VERBOSE=1'
    } >$INTERPRETER_BUILD_WRAPPER
    fail_panic "Can't create build wrapper: $INTERPRETER_BUILD_WRAPPER"
    run chmod +x $INTERPRETER_BUILD_WRAPPER
    fail_panic "Can't chmod +x build wrapper: '$INTERPRETER_BUILD_WRAPPER'"

    local INTERPRETER_SOURCE
    local PY_INTERPRETER_FNAME
    case $1 in
        windows*)
            INTERPRETER_SOURCE="$PYTHON_BUILD_UTILS_DIR_HOST/interpreter-winapi.c.${PYTHON_ABI}"
            PY_INTERPRETER_FNAME="python.exe"
            ;;
        *)
            INTERPRETER_SOURCE="$PYTHON_BUILD_UTILS_DIR_HOST/interpreter-posix.c.${PYTHON_ABI}"
            PY_INTERPRETER_FNAME="python"
            ;;
    esac
    run cp -p -T "$INTERPRETER_SOURCE" "$BUILDDIR_INTERPRETER/interpreter.c"
    fail_panic "Can't copy '$INTERPRETER_SOURCE' to '$BUILDDIR_INTERPRETER'"

    log "build python-$PYTHON_ABI interpreter for $1 ..."
    run $INTERPRETER_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI interpreter for $1"

    run cp -p -t "$OUTPUT_DIR" "$BUILDDIR_INTERPRETER/build/$PY_INTERPRETER_FNAME"
    fail_panic "Can't copy '$PY_INTERPRETER_FNAME' to '$OUTPUT_DIR'"

# Step 4: build python stdlib for host
    log "build python-$PYTHON_ABI stdlib for $1 ..."
    local PY_STDLIB_ZIPFILE="$OUTPUT_DIR/stdlib.zip"
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --py2 --pysrc-root $PYTHON_SRCDIR --output-zip $PY_STDLIB_ZIPFILE
        fail_panic "Can't build python-$PYTHON_ABI stdlib for $1"
    else
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --pysrc-root $PYTHON_SRCDIR --output-zip $PY_STDLIB_ZIPFILE
        fail_panic "Can't build python-$PYTHON_ABI stdlib for $1"
    fi
}

# $1: host tag
need_build_host_python ()
{
    bh_stamps_do "build-vendor-host-python$PYTHON_ABI-$1" build_host_python $1
}

# Install host Python binaries and support files to the NDK install dir.
# $1: host tag
install_host_python ()
{
    need_build_host_python $1

    local SRCDIR="$(python_build_install_dir $1)"
    local DSTDIR="$NDK_DIR/$(python_ndk_install_dir $1)"

    log "$(bh_host_text) python-$PYTHON_ABI: Installing"
    run copy_directory "$SRCDIR/opt/python$PYTHON_ABI" "$DSTDIR/opt/python$PYTHON_ABI"
}

# $1: host tag
need_install_host_python ()
{
    bh_stamps_do "install-vendor-host-python$PYTHON_ABI-$1" install_host_python $1
}

# Package host Python binaries into a tarball
# $1: host tag
package_host_python ()
{
    need_install_host_python $1

    local PACKAGENAME="ndk-vendor-host-python$PYTHON_ABI-$(install_dir_from_host_tag $SYSTEM).tar.xz"
    local PACKAGE="$PACKAGE_DIR/$PACKAGENAME"

    local BLDDIR="$(python_build_install_dir $1)"
    local SRCDIR="$(python_ndk_install_dir $1)"
    # This is similar to BLDDIR=${BLDDIR%%$SRCDIR} (and requires we use windows and not windows-x86)
    BLDDIR=$(echo "$BLDDIR" | sed "s/$(echo "$SRCDIR" | sed -e 's/\\/\\\\/g' -e 's/\//\\\//g' -e 's/&/\\\&/g')//g")

    log "$(bh_host_text) $PACKAGENAME: Packaging"
    run pack_archive "$PACKAGE" "$BLDDIR" "$SRCDIR"
    log "$(bh_host_text) $PACKAGENAME: Caching"
    run cache_package "$PACKAGE_DIR" "$PACKAGENAME"
}

# $1: host tag
need_package_host_python ()
{
    bh_stamps_do "package-vendor-host-python$PYTHON_ABI-$1" package_host_python $1
}

# Check for cached packages
NOT_CACHED_SYSTEMS=
for SYSTEM in $BH_HOST_SYSTEMS; do
    PACKAGENAME="ndk-vendor-host-python$PYTHON_ABI-$(install_dir_from_host_tag $SYSTEM).tar.xz"
    echo "Look for: $PACKAGENAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGENAME" no_exit
    if [ $? != 0 ]; then
        if [ -z $NOT_CACHED_SYSTEMS ] ; then
            NOT_CACHED_SYSTEMS=$SYSTEM
        else
            NOT_CACHED_SYSTEMS="$NOT_CACHED_SYSTEMS $SYSTEM"
        fi
    fi
done

if [ -z "$NOT_CACHED_SYSTEMS" ] ; then
    log "For all systems were found cached packages."
    exit 0
else
    log "Not cached systems: $NOT_CACHED_SYSTEMS"
    BH_HOST_SYSTEMS=$NOT_CACHED_SYSTEMS
fi

# Let's build this
for SYSTEM in $BH_HOST_SYSTEMS; do
    bh_setup_build_for_host $SYSTEM
    need_install_host_python $SYSTEM
done

if [ "$PACKAGE_DIR" ]; then
    for SYSTEM in $BH_HOST_SYSTEMS; do
        bh_setup_build_for_host $SYSTEM
        need_package_host_python $SYSTEM
    done
fi

log "Done!"
