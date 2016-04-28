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

# Generate shell wrapper for cmake
# $1: wrapper file name
# $2: optional path of file with cmake toolchain description
generate_cmake_wrapper ()
{
    local WRAPPER_FNAME=$1
    local CMAKE_TOOLCHAIN_DESCRIPTION=$2
    local TOOLCHAIN_OPTION
    if [ -n "$CMAKE_TOOLCHAIN_DESCRIPTION" ]; then
        TOOLCHAIN_OPTION="-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_DESCRIPTION"
    fi
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo 'DIR_BUILD="$DIR_HERE/build"'
        echo 'rm -rf $DID_BUILD && mkdir -p $DIR_BUILD && cd $DIR_BUILD'
        echo "cmake $TOOLCHAIN_OPTION \$DIR_HERE"
        echo 'make -j'$NUM_JOBS' VERBOSE=1'
    } >$WRAPPER_FNAME
    fail_panic "Can't create cmake build wrapper: '$WRAPPER_FNAME'"
    run chmod +x $WRAPPER_FNAME
    fail_panic "Can't chmod +x cmake build wrapper: '$WRAPPER_FNAME'"
}

# Build python stub for internal purposes
# $1: output directory
build_python_stub ()
{
    local BUILDDIR_PYSTUB="$1"
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
    generate_cmake_wrapper $PYSTUB_CORE_BUILD_WRAPPER

    run cp -p -T $PY_C_CONFIG_FILE "$BUILDDIR_PYSTUB_CORE/config.c" && \
        cp -p -t "$BUILDDIR_PYSTUB_CORE" "$BUILDDIR_PYSTUB_CONFIG/pyconfig.h"
    fail_panic "Can't copy config.c pyconfig.h to $BUILDDIR_PYSTUB_CORE"

    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PY_C_GETPATH="$PYTHON_BUILD_UTILS_DIR_HOST/getpath.c.$PYTHON_ABI"
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
        echo 'link_libraries(dl)'
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
        echo "set(CMAKE_C_FLAGS \"-DPYTHON_STDLIB_PATH=\\\\\\\"$PYTHON_SRCDIR/Lib\\\\\\\"\")"
    } >$PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_0
    fail_panic "Can't generate $PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_0"

    local PYSTUB_INTERPRETER_0_BUILD_WRAPPER="$BUILDDIR_PYSTUB_INTERPRETER_0/build.sh"
    generate_cmake_wrapper $PYSTUB_INTERPRETER_0_BUILD_WRAPPER

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
        echo 'link_libraries(dl)'
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
        echo "set(CMAKE_C_FLAGS \"-DPYTHON_STDLIB_PATH=\\\\\\\"stdlib.zip\\\\\\\"\")"
    } >$PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_1
    fail_panic "Can't generate $PYSTUB_INTERPRETER_CMAKE_DESCRIPTION_1"

    local PYSTUB_INTERPRETER_1_BUILD_WRAPPER="$BUILDDIR_PYSTUB_INTERPRETER_1/build.sh"
    generate_cmake_wrapper $PYSTUB_INTERPRETER_1_BUILD_WRAPPER

    run cp -p -T $PY_C_INTERPRETER_STUB_FILE "$BUILDDIR_PYSTUB_INTERPRETER_1/interpreter.c"
    fail_panic "Can't copy $PY_C_INTERPRETER_STUB_FILE to $BUILDDIR_PYSTUB_INTERPRETER_1"
    log "build python-$PYTHON_ABI interpreter(1) stub for $BH_BUILD_TAG ..."
    run $PYSTUB_INTERPRETER_1_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI interpreter(1) stub for $BH_BUILD_TAG"
    run cp -p -t "$BUILDDIR_PYSTUB_BIN" "$BUILDDIR_PYSTUB_INTERPRETER_1/build/python"
    fail_panic "Can't copy python from $BUILDDIR_PYSTUB_INTERPRETER_1/build to $BUILDDIR_PYSTUB_BIN"
}

# Build python module
# $1: host system tag
# $2: module name
# $3: sources, comma separated list
# $4: build dir
# $5: cmake toolchain file
# $6: include dirs, comma separated list
# $7: lib dir
# $8: output dir
build_python_module ()
{
    local HOST_SYSTEM_TAG=$1
    local MOD_NAME=$2
    local MOD_SOURCES=$(commas_to_spaces $3)
    local MOD_BUILD_DIR=$4
    local CMAKE_TOOLCHAIN_DESCRIPTION=$5
    local INCLUDE_DIR_LIST=$(commas_to_spaces $6)
    local LIB_DIR=$7
    local OUTPUT_DIR=$8
    local MODS_OUTPUT_DIR="$OUTPUT_DIR/modules"

    log "build python-$PYTHON_ABI module '$MOD_NAME' for $HOST_SYSTEM_TAG ..."

    run mkdir -p $MOD_BUILD_DIR
    fail_panic "Can't create directory: $MOD_BUILD_DIR"
    run mkdir -p $MODS_OUTPUT_DIR
    fail_panic "Can't create directory: $MODS_OUTPUT_DIR"

    local PYCORE_LIBNAME
    case $HOST_SYSTEM_TAG in
        windows*)
            PYCORE_LIBNAME="python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}"
            ;;
        *)
            if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
                PYCORE_LIBNAME="python${PYTHON_ABI}"
            else
                PYCORE_LIBNAME="python${PYTHON_ABI}m"
            fi
            ;;
    esac

    local MOD_LIBLIST="$PYCORE_LIBNAME"
    case $HOST_SYSTEM_TAG in
        windows*)
            case $MOD_NAME in
                _multiprocessing|_socket|select)
                    MOD_LIBLIST="$MOD_LIBLIST ws2_32"
                    ;;
            esac
            ;;
        *)
            case $MOD_NAME in
                _ctypes)
                    MOD_LIBLIST="$MOD_LIBLIST dl"
                    ;;
                _multiprocessing)
                    MOD_LIBLIST="$MOD_LIBLIST pthread"
                    ;;
            esac
            ;;
    esac

    local MOD_CMAKE_DESCRIPTION="$MOD_BUILD_DIR/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        if [ "$MOD_NAME" = "_ctypes" ]; then
            echo "enable_language(C ASM)"
            case $HOST_SYSTEM_TAG in
                windows*)
                    if [ "$HOST_SYSTEM_TAG" != "windows-x86_64" ]; then
                        echo 'set(CMAKE_ASM_FLAGS "-DSYMBOL_UNDERSCORE=1")'
                    fi
                    ;;
            esac
        elif [ "$MOD_NAME" = "pyexpat" ]; then
            case $HOST_SYSTEM_TAG in
                windows*)
                    echo 'set(CMAKE_C_FLAGS "-DCOMPILED_FROM_DSP -DXML_STATIC")'
                    ;;
                *)
                    echo 'set(CMAKE_C_FLAGS "-DHAVE_EXPAT_CONFIG_H -DXML_STATIC")'
                    ;;
            esac
        fi
        echo "set(MY_PYTHON_SRC_ROOT \"$PYTHON_SRCDIR\")"
        echo "set(CMAKE_BUILD_TYPE RELEASE)"
        echo "set(CMAKE_SKIP_RPATH TRUE)"
        echo ""
        echo "if(UNIX AND NOT APPLE)"
        echo "    set(LINUX TRUE)"
        echo "endif()"
        echo ""
        echo "if(LINUX)"
        echo "    set(CMAKE_SHARED_LINKER_FLAGS \"-Wl,--no-undefined\")"
        echo "endif()"
        echo ""
        echo "set(PYMOD_TARGET_NAME \"$MOD_NAME\")"
        echo ""
        echo "include_directories(\${MY_PYTHON_SRC_ROOT}/Include)"
        for incd in $INCLUDE_DIR_LIST; do
            case $incd in
                /*)
                    echo "include_directories($incd)"
                    ;;
                *)
                    echo "include_directories(\${MY_PYTHON_SRC_ROOT}/$incd)"
                    ;;
            esac
        done
        echo ""
        echo "link_directories(\"$LIB_DIR\")"
        echo ""
        echo "set(SRC_FILES"
        for src in $MOD_SOURCES; do
            case $src in
                /*)
                    echo "  $src"
                    ;;
                *)
                    echo "  \${MY_PYTHON_SRC_ROOT}/$src"
                    ;;
            esac
        done
        echo ")"
        echo ""
        echo "add_library(\${PYMOD_TARGET_NAME} SHARED \${SRC_FILES})"
        echo "target_link_libraries(\${PYMOD_TARGET_NAME} $MOD_LIBLIST)"
        echo "set_target_properties(\${PYMOD_TARGET_NAME} PROPERTIES DEFINE_SYMBOL \"Py_BUILD_CORE_MODULE\")"
        echo ""
        echo "if(WIN32)"
        echo "  set_target_properties(\${PYMOD_TARGET_NAME} PROPERTIES PREFIX \"\")"
        echo "endif()"
        echo ""
        echo "if (APPLE)"
        echo "  set_target_properties(\${PYMOD_TARGET_NAME} PROPERTIES SUFFIX \".so\")"
        echo "  add_custom_command(TARGET \${PYMOD_TARGET_NAME} POST_BUILD COMMAND"
        echo "    install_name_tool -id lib\${PYMOD_TARGET_NAME}.so lib\${PYMOD_TARGET_NAME}.so)"
        echo "  add_custom_command(TARGET \${PYMOD_TARGET_NAME} POST_BUILD COMMAND"
        echo "    install_name_tool -change lib${PYCORE_LIBNAME}.so @loader_path/../lib${PYCORE_LIBNAME}.so lib\${PYMOD_TARGET_NAME}.so)"
        echo "endif()"
    } >$MOD_CMAKE_DESCRIPTION
    fail_panic "Can't generate '$MOD_CMAKE_DESCRIPTION'"

    local MOD_BUILD_WRAPPER="$MOD_BUILD_DIR/build.sh"
    generate_cmake_wrapper $MOD_BUILD_WRAPPER $CMAKE_TOOLCHAIN_DESCRIPTION

    log "build python-$PYTHON_ABI module '$MOD_NAME' for $HOST_SYSTEM_TAG ..."
    run $MOD_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI module '$MOD_NAME' for $HOST_SYSTEM_TAG"

    local MOD_BUILD_FNAME
    local MOD_OUTPUT_FNAME
    case $HOST_SYSTEM_TAG in
        windows*)
            MOD_BUILD_FNAME="${MOD_NAME}.dll"
            MOD_OUTPUT_FNAME="${MOD_NAME}.pyd"
            ;;
        *)
            MOD_BUILD_FNAME="lib${MOD_NAME}.so"
            MOD_OUTPUT_FNAME="${MOD_NAME}.so"
            ;;
    esac

    run cp -p -T "$MOD_BUILD_DIR/build/$MOD_BUILD_FNAME" "$MODS_OUTPUT_DIR/$MOD_OUTPUT_FNAME"
    fail_panic "Can't copy '$MOD_BUILD_DIR/build/$MOD_BUILD_FNAME' as '$MODS_OUTPUT_DIR/$MOD_OUTPUT_FNAME'"
}

# Build Python binaries for host
# $1: host system tag
build_host_python ()
{
# Define directories to be used
    local HOST_SYSTEM_TAG=$1
    local OBJ_DIR="$BH_BUILD_DIR/py$PYTHON_ABI-$HOST_SYSTEM_TAG"
    run mkdir -p $OBJ_DIR
    fail_panic "Can't create directory: $OBJ_DIR"

    local CONFIG_INCLUDE_DIR="$OBJ_DIR/include"
    run mkdir -p $CONFIG_INCLUDE_DIR
    fail_panic "Can't create directory: $CONFIG_INCLUDE_DIR"

    local PY_HOST_LINK_LIB_DIR="$OBJ_DIR/lib"
    run mkdir -p $PY_HOST_LINK_LIB_DIR
    fail_panic "Can't create directory: $PY_HOST_LINK_LIB_DIR"

    local OUTPUT_DIR="$(python_build_install_dir $HOST_SYSTEM_TAG)/opt/python$PYTHON_ABI"
    run mkdir -p $OUTPUT_DIR
    fail_panic "Can't create directory: $OUTPUT_DIR"

    local MINGW_ROOT
    case $HOST_SYSTEM_TAG in
        windows*)
            MINGW_ROOT=$(cd "$MINGW_PATH/.." && pwd)
            if [ ! -d "$MINGW_ROOT" ]; then
                panic "MINGW root not a directory: '$MINGW_ROOT'"
            else
                log "MINGW: '$MINGW_ROOT'"
            fi
            ;;
    esac


# Step 1: build python stub once
    local PY_STUB_DIR="$BH_BUILD_DIR/py$PYTHON_ABI-stub"
    bh_stamps_do "build-vendor-python$PYTHON_ABI-stub" build_python_stub $PY_STUB_DIR
    local PYTHON_FOR_BUILD="$PY_STUB_DIR/bin/python"

# Step 2: prepare cmake toolchain wrappers for host
    local TOOLCHAIN_WRAPPER_PREFIX
    local CMAKE_CROSS_SYSTEM_NAME
    case $HOST_SYSTEM_TAG in
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
            panic "Unknown platform: $HOST_SYSTEM_TAG"
            ;;
    esac
    local CMAKE_TOOLCHAIN_DESCRIPTION="$OBJ_DIR/toolchain.cmake"
    {
        echo "set(CMAKE_SYSTEM_NAME $CMAKE_CROSS_SYSTEM_NAME)"
        echo "set(CMAKE_C_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc)"
        echo "set(CMAKE_CXX_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-g++)"
        case $HOST_SYSTEM_TAG in
            windows*)
                echo "set(CMAKE_RC_COMPILER $BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-windres)"
            ;;
        esac
    } >$CMAKE_TOOLCHAIN_DESCRIPTION
    fail_panic "Can't create cmake-toolchain wrapper: '$CMAKE_TOOLCHAIN_DESCRIPTION'"

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
    generate_cmake_wrapper $CORE_BUILD_WRAPPER $CMAKE_TOOLCHAIN_DESCRIPTION

    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PY_C_GETPATH="$PYTHON_BUILD_UTILS_DIR_HOST/getpath.c.$PYTHON_ABI"
        run cp -p -T $PY_C_GETPATH "$BUILDDIR_CORE/getpath.c"
        fail_panic "Can't copy $PY_C_GETPATH to $BUILDDIR_CORE"
    fi
    run cp -p -T $PY_C_CONFIG_FILE "$BUILDDIR_CORE/config.c"
    fail_panic "Can't copy config.c to $BUILDDIR_CORE"
    case $HOST_SYSTEM_TAG in
        windows*)
            run cp -p -t "$BUILDDIR_CORE" \
                "$PYTHON_SRCDIR/PC/pyconfig.h" \
                "$PYTHON_SRCDIR/PC/errmap.h" \
                "$PYTHON_SRCDIR/Python/importdl.h" \
                "$PYTHON_SRCDIR/Python/dynload_win.c" \
                "$PYTHON_SRCDIR/Modules/posixmodule.c"
            fail_panic "Can't copy pyconfig.h errmap.h importdl.h dynload_win.c posixmodule.c to $BUILDDIR_CORE"
            run patch "$BUILDDIR_CORE/pyconfig.h" < "$PYTHON_BUILD_UTILS_DIR_HOST/pyconfig.h.$PYTHON_ABI.mingw.patch"
            fail_panic "Can't patch pyconfig.h"
            if [ "$PYTHON_MAJOR_VERSION" == "3" ]; then
                run cp -p -t "$BUILDDIR_CORE" "$PYTHON_SRCDIR/PC/getpathp.c"
                fail_panic "Can't copy getpathp.c to $BUILDDIR_CORE"
            fi
            # fix CamelCase inclusions for windows.h and mstcpip.h
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
            run cp -p -t "$BUILDDIR_CORE" "$PY_STUB_DIR/config/pyconfig.h"
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
                echo "export RANLIB=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-ranlib\""
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
            log "configure python-$PYTHON_ABI for $HOST_SYSTEM_TAG ..."
            run $CONFIGURE_WRAPPER
            fail_panic "Can't configure python-$PYTHON_ABI for $HOST_SYSTEM_TAG"
            run cp -p -t "$BUILDDIR_CORE" "$BUILDDIR_CONFIG/pyconfig.h"
            fail_panic "Can't copy pyconfig.h to $BUILDDIR_CORE"
            ;;
        *)
            panic "Unknown platform: $HOST_SYSTEM_TAG"
            ;;
    esac

    log "build python-$PYTHON_ABI core for $HOST_SYSTEM_TAG ..."
    run $CORE_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI core for $HOST_SYSTEM_TAG"

    local PY_CORE_FNAME
    local PY_CORE_LIB_FNAME
    case $HOST_SYSTEM_TAG in
        windows*)
            PY_CORE_FNAME="python${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.dll"
            PY_CORE_LIB_FNAME="libpython${PYTHON_MAJOR_VERSION}${PYTHON_MINOR_VERSION}.dll.a"
            ;;
        *)
            if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
                PY_CORE_FNAME="libpython${PYTHON_ABI}.so"
            else
                PY_CORE_FNAME="libpython${PYTHON_ABI}m.so"
            fi
            PY_CORE_LIB_FNAME="$PY_CORE_FNAME"
            ;;
    esac
    run cp -p -t "$OUTPUT_DIR" "$BUILDDIR_CORE/build/$PY_CORE_FNAME"
    fail_panic "Can't copy '$PY_CORE_FNAME' to '$OUTPUT_DIR'"
    run cp -p -t "$PY_HOST_LINK_LIB_DIR" "$BUILDDIR_CORE/build/$PY_CORE_LIB_FNAME"
    fail_panic "Can't copy '$PY_CORE_LIB_FNAME' to '$PY_HOST_LINK_LIB_DIR'"
    run cp -p -t "$CONFIG_INCLUDE_DIR" "$BUILDDIR_CORE/pyconfig.h"
    fail_panic "Can't copy '$BUILDDIR_CORE/pyconfig.h' to 'CONFIG_INCLUDE_DIR'"

# Step 4: build python interpreter for host
    local BUILDDIR_INTERPRETER="$OBJ_DIR/interpreter"

    run mkdir -p $BUILDDIR_INTERPRETER
    fail_panic "Can't create directory: $BUILDDIR_INTERPRETER"
    local INTERPRETER_CMAKE_DESCRIPTION="$BUILDDIR_INTERPRETER/CMakeLists.txt"
    {
        echo "cmake_minimum_required (VERSION $CMAKE_MIN_VERSION)"
        echo 'set(CMAKE_BUILD_TYPE RELEASE)'
        case $HOST_SYSTEM_TAG in
            windows*)
                if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
                    echo 'set(CMAKE_EXE_LINKER_FLAGS "-municode")'
                fi
                ;;
            *)
                echo 'link_libraries(dl)'
                ;;
        esac
        echo "add_executable(python \${CMAKE_CURRENT_LIST_DIR}/interpreter.c)"
    } >$INTERPRETER_CMAKE_DESCRIPTION
    fail_panic "Can't generate $INTERPRETER_CMAKE_DESCRIPTION"

    local INTERPRETER_BUILD_WRAPPER="$BUILDDIR_INTERPRETER/build.sh"
    generate_cmake_wrapper $INTERPRETER_BUILD_WRAPPER $CMAKE_TOOLCHAIN_DESCRIPTION

    local INTERPRETER_SOURCE
    local PY_INTERPRETER_FNAME
    case $HOST_SYSTEM_TAG in
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

    log "build python-$PYTHON_ABI interpreter for $HOST_SYSTEM_TAG ..."
    run $INTERPRETER_BUILD_WRAPPER
    fail_panic "Can't build python-$PYTHON_ABI interpreter for $HOST_SYSTEM_TAG"

    run cp -p -t "$OUTPUT_DIR" "$BUILDDIR_INTERPRETER/build/$PY_INTERPRETER_FNAME"
    fail_panic "Can't copy '$PY_INTERPRETER_FNAME' to '$OUTPUT_DIR'"

# Step 5: build python stdlib for host
    log "build python-$PYTHON_ABI stdlib for $HOST_SYSTEM_TAG ..."
    local PY_STDLIB_ZIPFILE="$OUTPUT_DIR/stdlib.zip"
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --py2 --pysrc-root $PYTHON_SRCDIR --output-zip $PY_STDLIB_ZIPFILE
        fail_panic "Can't build python-$PYTHON_ABI stdlib for $HOST_SYSTEM_TAG"
    else
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --pysrc-root $PYTHON_SRCDIR --output-zip $PY_STDLIB_ZIPFILE
        fail_panic "Can't build python-$PYTHON_ABI stdlib for $HOST_SYSTEM_TAG"
    fi

# Step 6: build python modules
# _ctypes
    local BUILDDIR_CTYPES="$OBJ_DIR/ctypes"
    local BUILDDIR_CTYPES_CONFIG="$BUILDDIR_CTYPES/config"
    local BUILDDIR_CTYPES_INC="$BUILDDIR_CTYPES/include"
    run mkdir -p $BUILDDIR_CTYPES_CONFIG
    fail_panic "Can't create directory: $BUILDDIR_CTYPES_CONFIG"
    run mkdir -p $BUILDDIR_CTYPES_INC
    fail_panic "Can't create directory: $BUILDDIR_CTYPES_INC"
    local LIBFFI_CONFIGURE_WRAPPER="$BUILDDIR_CTYPES_CONFIG/configure.sh"
    {
        echo "#!/bin/bash -e"
        echo ''
        echo "export CC=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc\""
        echo "export CPP=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-gcc -E\""
        echo "export AR=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-ar\""
        echo "export RANLIB=\"$BH_BUILD_DIR/toolchain-wrappers/$TOOLCHAIN_WRAPPER_PREFIX-ranlib\""
        echo ''
        echo 'cd $(dirname $0)'
        echo ''
        echo "exec $PYTHON_SRCDIR/Modules/_ctypes/libffi/configure \\"
        echo "    --host=$BH_HOST_CONFIG \\"
        echo "    --build=$BH_BUILD_CONFIG \\"
        echo "    --prefix=$BUILDDIR_CTYPES_CONFIG/install \\"
    } >$LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't create configure wrapper for libffi: '$LIBFFI_CONFIGURE_WRAPPER'"
    chmod +x $LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't chmod +x configure wrapper for libffi"
    run $LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't configure libffi for $HOST_SYSTEM_TAG"
    run cp -p $BUILDDIR_CTYPES_CONFIG/fficonfig.h $BUILDDIR_CTYPES_CONFIG/include/*.h $BUILDDIR_CTYPES_INC
    fail_panic "Can't copy configured libffi headers to '$BUILDDIR_CTYPES_INC'"

    local CTYPES_SRC_LIST
    local CTYPES_INC_DIR_LIST="$CONFIG_INCLUDE_DIR,$BUILDDIR_CTYPES_INC"
    case $HOST_SYSTEM_TAG in
        windows*)
            run cp -p -t "$BUILDDIR_CTYPES" "$PYTHON_SRCDIR/Modules/_ctypes/callproc.c"
            fail_panic "Can't copy callproc.c to '$BUILDDIR_CTYPES'"
            run patch "$BUILDDIR_CTYPES/callproc.c" < "$PYTHON_BUILD_UTILS_DIR_HOST/callproc.c.$PYTHON_ABI.mingw.patch"
            fail_panic "Can't patch callproc.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,$BUILDDIR_CTYPES/callproc.c"
            CTYPES_INC_DIR_LIST="$CTYPES_INC_DIR_LIST,Modules/_ctypes"
            ;;
        *)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/callproc.c"
            ;;
    esac

    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/callbacks.c"
    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/cfield.c"
    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/malloc_closure.c"
    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/stgdict.c"
    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/_ctypes.c"
    CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/prep_cif.c"

    case $HOST_SYSTEM_TAG in
        linux-x86_64)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi64.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/unix64.S"
            ;;
        linux-x86)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/sysv.S"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/win32.S"
            ;;
        darwin-x86_64)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi64.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/darwin64.S"
            ;;
        darwin-x86)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/darwin.S"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/win32.S"
            ;;
        windows-x86_64)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/win64.S"
            ;;
        windows*)
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/ffi.c"
            CTYPES_SRC_LIST="$CTYPES_SRC_LIST,Modules/_ctypes/libffi/src/x86/win32.S"
            ;;
    esac

    build_python_module $HOST_SYSTEM_TAG '_ctypes' $CTYPES_SRC_LIST $BUILDDIR_CTYPES \
        $CMAKE_TOOLCHAIN_DESCRIPTION $CTYPES_INC_DIR_LIST $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR

# _multiprocessing
    local MULTIPROCESSING_SRC_LIST
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        MULTIPROCESSING_SRC_LIST="$MULTIPROCESSING_SRC_LIST,Modules/_multiprocessing/socket_connection.c"
        case $HOST_SYSTEM_TAG in
            windows*)
                MULTIPROCESSING_SRC_LIST="$MULTIPROCESSING_SRC_LIST,Modules/_multiprocessing/win32_functions.c"
                MULTIPROCESSING_SRC_LIST="$MULTIPROCESSING_SRC_LIST,Modules/_multiprocessing/pipe_connection.c"
            ;;
        esac
    fi
    MULTIPROCESSING_SRC_LIST="$MULTIPROCESSING_SRC_LIST,Modules/_multiprocessing/multiprocessing.c"
    MULTIPROCESSING_SRC_LIST="$MULTIPROCESSING_SRC_LIST,Modules/_multiprocessing/semaphore.c"

    build_python_module $HOST_SYSTEM_TAG '_multiprocessing' $MULTIPROCESSING_SRC_LIST "$OBJ_DIR/multiprocessing" \
        $CMAKE_TOOLCHAIN_DESCRIPTION $CONFIG_INCLUDE_DIR $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR

# _socket
    local BUILDDIR_SOCKET="$OBJ_DIR/socket"
    run mkdir -p $BUILDDIR_SOCKET
    fail_panic "Can't create directory: $BUILDDIR_SOCKET"
    local SOCKET_INC_DIR_LIST="$CONFIG_INCLUDE_DIR"
    case $HOST_SYSTEM_TAG in
        windows*)
            SOCKET_INC_DIR_LIST="$SOCKET_INC_DIR_LIST,$BUILDDIR_SOCKET"
            # fix CamelCase inclusions for mstcpip.h
            run find $MINGW_ROOT -name "mstcpip.h" -exec ln -s {} "$BUILDDIR_SOCKET/MSTcpIP.h" \;
            fail_panic "Can't create symlink for mstcpip.h"
        ;;
    esac

    build_python_module $HOST_SYSTEM_TAG '_socket' 'Modules/socketmodule.c' "$BUILDDIR_SOCKET" \
        $CMAKE_TOOLCHAIN_DESCRIPTION $SOCKET_INC_DIR_LIST $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR

# pyexpat
    local PYEXPAT_SRC_LIST
    PYEXPAT_SRC_LIST="$PYEXPAT_SRC_LIST,Modules/expat/xmlparse.c"
    PYEXPAT_SRC_LIST="$PYEXPAT_SRC_LIST,Modules/expat/xmlrole.c"
    PYEXPAT_SRC_LIST="$PYEXPAT_SRC_LIST,Modules/expat/xmltok.c"
    PYEXPAT_SRC_LIST="$PYEXPAT_SRC_LIST,Modules/pyexpat.c"

    build_python_module $HOST_SYSTEM_TAG 'pyexpat' $PYEXPAT_SRC_LIST "$OBJ_DIR/pyexpat" \
        $CMAKE_TOOLCHAIN_DESCRIPTION "$CONFIG_INCLUDE_DIR,Modules/expat" $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR

# select
    build_python_module $HOST_SYSTEM_TAG 'select' 'Modules/selectmodule.c' "$OBJ_DIR/select" \
        $CMAKE_TOOLCHAIN_DESCRIPTION $CONFIG_INCLUDE_DIR $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR

# unicodedata
    build_python_module $HOST_SYSTEM_TAG 'unicodedata' 'Modules/unicodedata.c' "$OBJ_DIR/unicodedata" \
        $CMAKE_TOOLCHAIN_DESCRIPTION $CONFIG_INCLUDE_DIR $PY_HOST_LINK_LIB_DIR $OUTPUT_DIR
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

if [ -z "$NOT_CACHED_SYSTEMS" ]; then
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

if [ -n "$PACKAGE_DIR" ]; then
    for SYSTEM in $BH_HOST_SYSTEMS; do
        bh_setup_build_for_host $SYSTEM
        need_package_host_python $SYSTEM
    done
fi

log "Done!"
