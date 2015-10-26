#!/bin/bash

# Copyright (c) 2011-2015 CrystaX .NET.
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
# THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX .NET.

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir>"

PROGRAM_DESCRIPTION=\
"Rebuild python libraries for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$PYTHON_SUBDIR, but you can override this with the --out-dir=<path>
option.
"

PACKAGE_DIR=
register_var_option "--package-dir=<path>" PACKAGE_DIR "Put prebuilt tarballs into <path>"

NDK_DIR=$ANDROID_NDK_ROOT
register_var_option "--ndk-dir=<path>" NDK_DIR "Specify NDK root path for the build"

BUILD_DIR=
OPTION_BUILD_DIR=
register_var_option "--build-dir=<path>" OPTION_BUILD_DIR "Specify temporary build dir"

ABIS=$PREBUILT_ABIS
register_var_option "--abis=<list>" ABIS "Specify list of target ABIs"

register_jobs_option

extract_parameters "$@"

PYTHON_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$PYTHON_SRCDIR" ]; then
    echo "ERROR: Please provide the path to the python source tree. See --help"
    exit 1
fi

if [ ! -d "$PYTHON_SRCDIR" ]; then
    echo "ERROR: No such directory: '$PYTHON_SRCDIR'"
    exit 1
fi

PYTHON_SRCDIR=$(cd $PYTHON_SRCDIR && pwd)

PYTHON_MAJOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MAJOR_VERSION[ \t]*\([0-9]*\).*/\1/p')

PYTHON_MINOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MINOR_VERSION[ \t]*\([0-9]*\).*/\1/p')

if [ -z "$PYTHON_MAJOR_VERSION" ]; then
    echo "ERROR: Can't detect python major version." 1>&2
    exit 1
fi

if [ -z "$PYTHON_MINOR_VERSION" ]; then
    echo "ERROR: Can't detect python minor version." 1>&2
    exit 1
fi

PYTHON_ABI="$PYTHON_MAJOR_VERSION"'.'"$PYTHON_MINOR_VERSION"
PYTHON_DSTDIR=$NDK_DIR/$PYTHON_SUBDIR/$PYTHON_ABI
mkdir -p $PYTHON_DSTDIR
fail_panic "Can't create python destination directory: $PYTHON_DSTDIR"

PYTHON_BUILD_UTILS_DIR=$(cd $(dirname $0)/build-target-python && pwd)
if [ ! -d "$PYTHON_BUILD_UTILS_DIR" ]; then
    echo "ERROR: No such directory: '$PYTHON_BUILD_UTILS_DIR'"
    exit 1
fi

PY_C_CONFIG_FILE="$PYTHON_BUILD_UTILS_DIR/config.c.$PYTHON_ABI"
if [ ! -f "$PY_C_CONFIG_FILE" ]; then
    echo "ERROR: Build of python $PYTHON_ABI is not supported, no such file: $PY_C_CONFIG_FILE"
    exit 1
fi

PY_C_INTERPRETER_FILE="$PYTHON_BUILD_UTILS_DIR/interpreter.c.$PYTHON_ABI"
if [ ! -f "$PY_C_INTERPRETER_FILE" ]; then
    echo "ERROR: Build of python $PYTHON_ABI is not supported, no such file: $PY_C_INTERPRETER_FILE"
    exit 1
fi

PY_ANDROID_MK_TEMPLATE_FILE="$PYTHON_BUILD_UTILS_DIR/android.mk.$PYTHON_ABI"
if [ ! -f "$PY_ANDROID_MK_TEMPLATE_FILE" ]; then
    echo "ERROR: Build of python $PYTHON_ABI is not supported, no such file: $PY_ANDROID_MK_TEMPLATE_FILE"
    exit 1
fi

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-python
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Could not create build directory: $BUILD_DIR"

# $1: ABI
# $2: build directory
build_python_for_abi ()
{
    local ABI="$1"
    local BUILDDIR="$2"

    dump "Building python$PYTHON_ABI for $ABI"

	local BUILDDIR_CONFIG="$BUILDDIR/config"
	local BUILDDIR_CORE="$BUILDDIR/core"
	local BUILDDIR_INTERPRETER="$BUILDDIR/interpreter"

    run mkdir -p $BUILDDIR_CONFIG
    fail_panic "Could not create directory: $BUILDDIR_CONFIG"
    run mkdir -p $BUILDDIR_CORE
    fail_panic "Could not create directory: $BUILDDIR_CORE"
    run mkdir -p $BUILDDIR_INTERPRETER
    fail_panic "Could not create directory: $BUILDDIR_INTERPRETER"

    local OBJDIR_CORE=$BUILDDIR_CORE/obj/local/$ABI
    local OBJDIR_INTERPRETER=$BUILDDIR_INTERPRETER/obj/local/$ABI

	local PYBIN_INSTALLDIR=$PYTHON_DSTDIR/libs/$ABI

# Step 1: configure

	local BUILD_ON_PLATFORM=$($PYTHON_SRCDIR/config.guess)
	if [ -z "$BUILD_ON_PLATFORM" ]; then
        echo "ERROR: Can't resolve platform being built python on." 1>&2
        exit 1
    fi

    local ARCH
    case $ABI in
        armeabi*)
            ARCH=arm
            ;;
        arm64*)
            ARCH=arm64
            ;;
        x86|x86_64|mips|mips64)
            ARCH=$ABI
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local HOST
    case $ABI in
        armeabi*)
            HOST=arm-linux-androideabi
            ;;
        arm64*)
            HOST=aarch64-linux-android
            ;;
        x86)
            HOST=i686-linux-android
            ;;
        x86_64)
            HOST=x86_64-linux-android
            ;;
        mips)
            HOST=mipsel-linux-android
            ;;
        mips64)
            HOST=mips64el-linux-android
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local APILEVEL
    case $ABI in
        armeabi*|x86|mips)
            APILEVEL=9
            ;;
        arm64*|x86_64|mips64)
            APILEVEL=21
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    local TOOLCHAIN
    case $ABI in
        armeabi*)
            TOOLCHAIN=arm-linux-androideabi
            ;;
        x86)
            TOOLCHAIN=x86
            ;;
        mips)
            TOOLCHAIN=mipsel-linux-android
            ;;
        arm64-v8a)
            TOOLCHAIN=aarch64-linux-android
            ;;
        x86_64)
            TOOLCHAIN=x86_64
            ;;
        mips64)
            TOOLCHAIN=mips64el-linux-android
            ;;
        *)
            echo "ERROR: Unknown ABI: '$ABI'" 1>&2
            exit 1
    esac

    case $ABI in
        armeabi)
            CFLAGS="-march=armv5te -mtune=xscale -msoft-float"
            ;;
        armeabi-v7a)
            CFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp"
            ;;
        armeabi-v7a-hard)
            CFLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mhard-float"
            ;;
        *)
            CFLAGS=""
    esac

    case $ABI in
        armeabi*)
            CFLAGS="$CFLAGS -mthumb"
    esac

    local CFLAGS="$CFLAGS --sysroot=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH"

    local LDFLAGS=""
    if [ "$ABI" = "armeabi-v7a-hard" ]; then
        LDFLAGS="$LDFLAGS -Wl,--no-warn-mismatch"
    fi
    LDFLAGS="$LDFLAGS -L$NDK_DIR/sources/crystax/libs/$ABI --sysroot=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH"

    local TCPREFIX=$NDK_DIR/toolchains/${TOOLCHAIN}-4.9/prebuilt/$HOST_TAG


    local CC=$TCPREFIX/bin/${HOST}-gcc
    local CPP="$CC $CFLAGS -E"
    local AR=$TCPREFIX/bin/${HOST}-ar
    local RANLIB=$TCPREFIX/bin/${HOST}-ranlib
    local READELF=$TCPREFIX/bin/${HOST}-readelf
    local PYTHON_FOR_BUILD=$NDK_DIR/prebuilt/$HOST_TAG/bin/python
    local CONFIG_SITE=$PYTHON_TOOLS_DIR/config.site

    local CONFIG_SITE=$BUILDDIR_CONFIG/config.site
    {
        echo 'ac_cv_file__dev_ptmx=no'
        echo 'ac_cv_file__dev_ptc=no'
        echo 'ac_cv_func_faccessat=no'
    } >$CONFIG_SITE
    fail_panic "Can't create config.site wrapper"

    local CONFIGURE_WRAPPER=$BUILDDIR_CONFIG/configure.sh
    {
        echo "#!/bin/bash -e"
        echo ''
        echo "export CC=\"$CC\""
        echo "export CPP=\"$CPP\""
        echo "export AR=\"$AR\""
        echo "export CFLAGS=\"$CFLAGS\""
        echo "export LDFLAGS=\"$LDFLAGS\""
        echo "export RANLIB=\"$RANLIB\""
        echo "export READELF=\"$READELF\""
        echo "export PYTHON_FOR_BUILD=\"$PYTHON_FOR_BUILD\""
        echo "export CONFIG_SITE=\"$CONFIG_SITE\""
        echo ''
        echo 'cd $(dirname $0)'
        echo ''
        echo "exec $PYTHON_SRCDIR/configure \\"
        echo "    --host=$HOST \\"
        echo "    --build=$BUILD_ON_PLATFORM \\"
        echo "    --prefix=$BUILDDIR/install \\"
        echo "    --enable-shared \\"
        echo "    --with-threads \\"
        echo "    --enable-ipv6 \\"
        echo "    --with-computed-gotos \\"
        echo "    --without-ensurepip"
    } >$CONFIGURE_WRAPPER
    fail_panic "Can't create configure wrapper"

    chmod +x $CONFIGURE_WRAPPER
    fail_panic "Can't chmod +x configure wrapper"

    run $CONFIGURE_WRAPPER
    fail_panic "Can't configure python$PYTHON_ABI for $ABI"

# Step 2: build python-core

    run mkdir -p $BUILDDIR_CORE/jni
    fail_panic "Could not create directory: $BUILDDIR_CORE/jni"

    run cp -p -T $PY_C_CONFIG_FILE $BUILDDIR_CORE/jni/config.c && \
        cp -p -t $BUILDDIR_CORE/jni $PYTHON_BUILD_UTILS_DIR/pyconfig.h
    fail_panic "Could not copy config.c pyconfig.h to $BUILDDIR_CORE/jni"

    local PYCONFIG_FOR_ABI="$BUILDDIR_CORE/jni/pyconfig_$(echo $ABI | tr '-' '_').h"
    run cp -p -T $BUILDDIR_CONFIG/pyconfig.h $PYCONFIG_FOR_ABI
    fail_panic "Could not copy $BUILDDIR_CONFIG/pyconfig.h to $PYCONFIG_FOR_ABI"

    local PYTHON_CORE_MODULE_NAME='python'"$PYTHON_ABI"'m'
    local PYTHON_SOABI='cpython-'"$PYTHON_ABI"'m'
    {
         echo 'LOCAL_PATH := $(call my-dir)'
         echo 'include $(CLEAR_VARS)'
         echo "LOCAL_MODULE := $PYTHON_CORE_MODULE_NAME"
		 echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
         echo 'LOCAL_C_INCLUDES := $(MY_PYTHON_SRC_ROOT)/Include'
         echo "LOCAL_CFLAGS := -DSOABI=\\\"$PYTHON_SOABI\\\" -DPy_BUILD_CORE -DPy_ENABLE_SHARED -DPLATFORM=\\\"linux\\\""
         echo 'LOCAL_LDLIBS := -lz'
         cat $PY_ANDROID_MK_TEMPLATE_FILE
         echo 'include $(BUILD_SHARED_LIBRARY)'
    } >$BUILDDIR_CORE/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_CORE/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_CORE -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI core"

    if [ "$PYTHON_HEADERS_INSTALLED" != "yes" ]; then
        log "Install python$PYTHON_ABI headers into $PYTHON_DSTDIR"
        run rm -Rf $PYTHON_DSTDIR/include
        run mkdir -p $PYTHON_DSTDIR/include/python && \
        run cp -p $PYTHON_BUILD_UTILS_DIR/pyconfig.h $PYTHON_SRCDIR/Include/*.h $PYTHON_DSTDIR/include/python
        fail_panic "Can't install python$PYTHON_ABI headers"
        PYTHON_HEADERS_INSTALLED=yes
        export PYTHON_HEADERS_INSTALLED
    fi
    log "Install $(basename $PYCONFIG_FOR_ABI) into $PYTHON_DSTDIR"
    run cp -p $PYCONFIG_FOR_ABI $PYTHON_DSTDIR/include/python
    fail_panic "Can't install $PYCONFIG_FOR_ABI"

    run mkdir -p $PYBIN_INSTALLDIR
    fail_panic "Can't create $PYBIN_INSTALLDIR"

    log "Install python$PYTHON_ABI-$ABI core in $PYBIN_INSTALLDIR"
    run cp -fpH $OBJDIR_CORE/lib$PYTHON_CORE_MODULE_NAME.so $PYBIN_INSTALLDIR
    fail_panic "Can't install python$PYTHON_ABI-$ABI core in $PYBIN_INSTALLDIR"

# Step 3: build python-interpreter

    run mkdir -p $BUILDDIR_INTERPRETER/jni
    fail_panic "Could not create directory: $BUILDDIR_INTERPRETER/jni"

    run cp -p -T $PY_C_INTERPRETER_FILE $BUILDDIR_INTERPRETER/jni/interpreter.c
    fail_panic "Could not copy $PY_C_INTERPRETER_FILE to $BUILDDIR_INTERPRETER/jni"

    {
         echo 'LOCAL_PATH := $(call my-dir)'
         echo 'include $(CLEAR_VARS)'
         echo 'LOCAL_MODULE := python'
         echo 'LOCAL_SRC_FILES := interpreter.c'
         echo 'include $(BUILD_EXECUTABLE)'
    } >$BUILDDIR_INTERPRETER/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_INTERPRETER/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_INTERPRETER -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI interpreter"

    log "Install python$PYTHON_ABI-$ABI interpreter in $PYBIN_INSTALLDIR"
    run cp -fpH $OBJDIR_INTERPRETER/python $PYBIN_INSTALLDIR
    fail_panic "Can't install python$PYTHON_ABI-$ABI interpreter in $PYBIN_INSTALLDIR"
}

if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}-headers.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        PYTHON_HEADERS_NEED_PACKAGE=no
    else
        PYTHON_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}-libs-${ABI}.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$PYTHON_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_python_for_abi $ABI "$BUILD_DIR/$ABI"
    fi
done

if [ -n "$PACKAGE_DIR" ]; then
    if [ "$PYTHON_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$PYTHON_SUBDIR/$PYTHON_ABI/include"
        PACKAGE_NAME="python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}-headers.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package python headers"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES="$PYTHON_SUBDIR/$PYTHON_ABI/libs/$ABI"
        PACKAGE_NAME="python${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}-libs-${ABI}.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package python $ABI libs"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    done
fi

if [ -z "$OPTION_BUILD_DIR" ]; then
    log "Cleaning up..."
    rm -rf $BUILD_DIR
else
    log "Don't forget to cleanup: $BUILD_DIR"
fi

log "Done!"
