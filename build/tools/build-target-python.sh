#!/bin/bash

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
    panic "Please provide the path to the python source tree. See --help"
fi

if [ ! -d "$PYTHON_SRCDIR" ]; then
    panic "No such directory: '$PYTHON_SRCDIR'"
fi

PYTHON_SRCDIR=$(cd $PYTHON_SRCDIR && pwd)

PYTHON_MAJOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MAJOR_VERSION[ \t]*\([0-9]*\).*/\1/p')

PYTHON_MINOR_VERSION=\
$(cat $PYTHON_SRCDIR/Include/patchlevel.h | sed -n 's/#define[ \t]*PY_MINOR_VERSION[ \t]*\([0-9]*\).*/\1/p')

if [ -z "$PYTHON_MAJOR_VERSION" ]; then
    panic "Can't detect python major version."
fi

if [ -z "$PYTHON_MINOR_VERSION" ]; then
    panic "Can't detect python minor version."
fi

PYTHON_ABI="${PYTHON_MAJOR_VERSION}.${PYTHON_MINOR_VERSION}"

PYTHON_FOR_BUILD="$NDK_DIR/prebuilt/$HOST_TAG/opt/python${PYTHON_ABI}/python"
if [ ! -f "$PYTHON_FOR_BUILD" ]; then
    panic "Can't find python for build: '$PYTHON_FOR_BUILD'."
fi

PYTHON_DSTDIR=$NDK_DIR/$PYTHON_SUBDIR/$PYTHON_ABI
mkdir -p $PYTHON_DSTDIR
fail_panic "Can't create python destination directory: $PYTHON_DSTDIR"

PYTHON_BUILD_UTILS_DIR=$(cd $(dirname $0)/build-python && pwd)
if [ ! -d "$PYTHON_BUILD_UTILS_DIR" ]; then
    panic "No such directory: '$PYTHON_BUILD_UTILS_DIR'"
fi

PY_C_CONFIG_FILE="$PYTHON_BUILD_UTILS_DIR/config.c.$PYTHON_ABI"
if [ ! -f "$PY_C_CONFIG_FILE" ]; then
    panic "Build of python $PYTHON_ABI is not supported, no such file: $PY_C_CONFIG_FILE"
fi

PY_C_INTERPRETER_FILE="$PYTHON_BUILD_UTILS_DIR/interpreter.c.$PYTHON_ABI"
if [ ! -f "$PY_C_INTERPRETER_FILE" ]; then
    panic "Build of python $PYTHON_ABI is not supported, no such file: $PY_C_INTERPRETER_FILE"
fi

PY_ANDROID_MK_TEMPLATE_FILE="$PYTHON_BUILD_UTILS_DIR/android.mk.$PYTHON_ABI"
if [ ! -f "$PY_ANDROID_MK_TEMPLATE_FILE" ]; then
    panic "Build of python $PYTHON_ABI is not supported, no such file: $PY_ANDROID_MK_TEMPLATE_FILE"
fi

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR=$NDK_TMPDIR/build-python
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Can't create build directory: $BUILD_DIR"

OPENSSL_HOME=''
if [ -n "$DEFAULT_OPENSSL_VERSION" ]; then
    if [ -f "$NDK_DIR/$OPENSSL_SUBDIR/$DEFAULT_OPENSSL_VERSION/Android.mk" \
         -a -f "$NDK_DIR/$OPENSSL_SUBDIR/$DEFAULT_OPENSSL_VERSION/include/openssl/opensslconf.h" ]; then
        OPENSSL_HOME="openssl/$DEFAULT_OPENSSL_VERSION"
    fi
fi

# $1: ABI
# $2: build directory
build_python_for_abi ()
{
    local ABI="$1"
    local BUILDDIR="$2"
    local PYBIN_INSTALLDIR="$PYTHON_DSTDIR/shared/$ABI"
    local PYBIN_INSTALLDIR_MODULES="$PYBIN_INSTALLDIR/modules"
    if [ -n "$OPENSSL_HOME" ]; then
        log "Building python$PYTHON_ABI for $ABI (with OpenSSL-$DEFAULT_OPENSSL_VERSION)"
    else
        log "Building python$PYTHON_ABI for $ABI (without OpenSSL support)"
    fi

# Step 1: configure
    local BUILDDIR_CONFIG="$BUILDDIR/config"
    local BUILDDIR_CORE="$BUILDDIR/core"
    local OBJDIR_CORE="$BUILDDIR_CORE/obj/local/$ABI"

    run mkdir -p $BUILDDIR_CONFIG
    fail_panic "Can't create directory: $BUILDDIR_CONFIG"
    run mkdir -p $BUILDDIR_CORE
    fail_panic "Can't create directory: $BUILDDIR_CORE"

    local BUILD_ON_PLATFORM=$($PYTHON_SRCDIR/config.guess)
    if [ -z "$BUILD_ON_PLATFORM" ]; then
        panic "Can't resolve platform being built python on."
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
            panic "Unknown ABI: '$ABI'"
            ;;
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
            panic "Unknown ABI: '$ABI'"
            ;;
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
            panic "Unknown ABI: '$ABI'"
            ;;
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
            panic "Unknown ABI: '$ABI'"
            ;;
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
            ;;
    esac

    case $ABI in
        armeabi*)
            CFLAGS="$CFLAGS -mthumb"
            ;;
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
    local CONFIG_SITE=$PYTHON_TOOLS_DIR/config.site

    local CONFIG_SITE=$BUILDDIR_CONFIG/config.site
    {
        echo 'ac_cv_file__dev_ptmx=no'
        echo 'ac_cv_file__dev_ptc=no'
        echo 'ac_cv_func_gethostbyname_r=no'
        if [ "$PYTHON_MAJOR_VERSION" == "3" ]; then
            echo 'ac_cv_func_faccessat=no'
        fi
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
        if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
            echo "exec $PYTHON_SRCDIR/configure \\"
            echo "    --host=$HOST \\"
            echo "    --build=$BUILD_ON_PLATFORM \\"
            echo "    --prefix=$BUILDDIR_CONFIG/install \\"
            echo "    --enable-shared \\"
            echo "    --with-threads \\"
            echo "    --enable-ipv6 \\"
            echo "    --enable-unicode=ucs4 \\"
            echo "    --without-ensurepip"
        else
            echo "exec $PYTHON_SRCDIR/configure \\"
            echo "    --host=$HOST \\"
            echo "    --build=$BUILD_ON_PLATFORM \\"
            echo "    --prefix=$BUILDDIR_CONFIG/install \\"
            echo "    --enable-shared \\"
            echo "    --with-threads \\"
            echo "    --enable-ipv6 \\"
            echo "    --with-computed-gotos \\"
            echo "    --without-ensurepip"
        fi
    } >$CONFIGURE_WRAPPER
    fail_panic "Can't create configure wrapper"

    chmod +x $CONFIGURE_WRAPPER
    fail_panic "Can't chmod +x configure wrapper"

    run $CONFIGURE_WRAPPER
    fail_panic "Can't configure python$PYTHON_ABI for $ABI"

# Step 2: build python-core
    run mkdir -p $BUILDDIR_CORE/jni
    fail_panic "Can't create directory: $BUILDDIR_CORE/jni"

    run cp -p -T $PY_C_CONFIG_FILE "$BUILDDIR_CORE/jni/config.c" && \
        cp -p -t "$BUILDDIR_CORE/jni" "$PYTHON_BUILD_UTILS_DIR/pyconfig.h"
    fail_panic "Can't copy config.c pyconfig.h to $BUILDDIR_CORE/jni"

    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PY_C_GETPATH="$PYTHON_BUILD_UTILS_DIR/getpath.c.$PYTHON_ABI"
        run cp -p -T $PY_C_GETPATH "$BUILDDIR_CORE/jni/getpath.c"
        fail_panic "Can't copy $PY_C_GETPATH to $BUILDDIR_CORE/jni"
    fi

    local PYCONFIG_FOR_ABI="$BUILDDIR_CORE/jni/pyconfig_$(echo $ABI | tr '-' '_').h"
    run cp -p -T $BUILDDIR_CONFIG/pyconfig.h $PYCONFIG_FOR_ABI
    fail_panic "Can't copy $BUILDDIR_CONFIG/pyconfig.h to $PYCONFIG_FOR_ABI"

    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        local PYTHON_CORE_MODULE_NAME='python'"$PYTHON_ABI"
    else
        local PYTHON_CORE_MODULE_NAME='python'"$PYTHON_ABI"'m'
        local PYTHON_SOABI='cpython-'"$PYTHON_ABI"'m'
    fi
    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo "LOCAL_MODULE := $PYTHON_CORE_MODULE_NAME"
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_C_INCLUDES := $(MY_PYTHON_SRC_ROOT)/Include'
        if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
            echo "LOCAL_CFLAGS := -DPy_BUILD_CORE -DPy_ENABLE_SHARED -DPLATFORM=\\\"linux\\\""
        else
            echo "LOCAL_CFLAGS := -DSOABI=\\\"$PYTHON_SOABI\\\" -DPy_BUILD_CORE -DPy_ENABLE_SHARED -DPLATFORM=\\\"linux\\\""
        fi
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
    run mkdir -p $PYBIN_INSTALLDIR_MODULES
    fail_panic "Can't create directory: $PYBIN_INSTALLDIR_MODULES"

    log "Install python$PYTHON_ABI-$ABI core in $PYBIN_INSTALLDIR"
    run cp -fpH $OBJDIR_CORE/lib$PYTHON_CORE_MODULE_NAME.so $PYBIN_INSTALLDIR
    fail_panic "Can't install python$PYTHON_ABI-$ABI core in $PYBIN_INSTALLDIR"

# Step 3: build python-interpreter
    local BUILDDIR_INTERPRETER="$BUILDDIR/interpreter"
    local OBJDIR_INTERPRETER="$BUILDDIR_INTERPRETER/obj/local/$ABI"

    run mkdir -p $BUILDDIR_INTERPRETER/jni
    fail_panic "Can't create directory: $BUILDDIR_INTERPRETER/jni"

    run cp -p -T $PY_C_INTERPRETER_FILE $BUILDDIR_INTERPRETER/jni/interpreter.c
    fail_panic "Can't copy $PY_C_INTERPRETER_FILE to $BUILDDIR_INTERPRETER/jni"

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

# Step 4: build python stdlib
    local PYSTDLIB_ZIPFILE="$PYBIN_INSTALLDIR/stdlib.zip"
    log "Install python$PYTHON_ABI-$ABI stdlib as $PYSTDLIB_ZIPFILE"
    if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --py2 --pysrc-root $PYTHON_SRCDIR --output-zip $PYSTDLIB_ZIPFILE
        fail_panic "Can't install python$PYTHON_ABI-$ABI stdlib"
    else
        run $PYTHON_FOR_BUILD $PYTHON_BUILD_UTILS_DIR/build_stdlib.py --pysrc-root $PYTHON_SRCDIR --output-zip $PYSTDLIB_ZIPFILE
        fail_panic "Can't install python$PYTHON_ABI-$ABI stdlib"
    fi

# Step 5: site-packages
    local SITE_README_SRCDIR="$PYTHON_SRCDIR/Lib/site-packages"
    local SITE_README_DSTDIR="$PYBIN_INSTALLDIR/site-packages"
    log "Install python$PYTHON_ABI-$ABI site-packages"
    run mkdir -p $SITE_README_DSTDIR && cp -fpH $SITE_README_SRCDIR/README $SITE_README_DSTDIR
    fail_panic "Can't install python$PYTHON_ABI-$ABI site-packages"

# Step 6: build python modules
# _ctypes
    local BUILDDIR_CTYPES="$BUILDDIR/ctypes"
    local OBJDIR_CTYPES="$BUILDDIR_CTYPES/obj/local/$ABI"
    local BUILDDIR_CTYPES_CONFIG="$BUILDDIR_CTYPES/config"
    run mkdir -p $BUILDDIR_CTYPES_CONFIG
    fail_panic "Can't create directory: $BUILDDIR_CTYPES_CONFIG"

    local LIBFFI_CONFIGURE_WRAPPER=$BUILDDIR_CTYPES_CONFIG/configure.sh
    {
        echo "#!/bin/bash -e"
        echo ''
        echo "export CC=\"$CC\""
        echo "export CFLAGS=\"$CFLAGS\""
        echo "export LDFLAGS=\"$LDFLAGS\""
        echo "export CPP=\"$CPP\""
        echo "export AR=\"$AR\""
        echo "export RANLIB=\"$RANLIB\""
        echo ''
        echo 'cd $(dirname $0)'
        echo ''
        echo "exec $PYTHON_SRCDIR/Modules/_ctypes/libffi/configure \\"
        echo "    --host=$HOST \\"
        echo "    --build=$BUILD_ON_PLATFORM \\"
        echo "    --prefix=$BUILDDIR_CTYPES_CONFIG/install \\"
    } >$LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't create configure wrapper for libffi"

    chmod +x $LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't chmod +x configure wrapper for libffi"

    run $LIBFFI_CONFIGURE_WRAPPER
    fail_panic "Can't configure libffi for $ABI"

    run mkdir -p "$BUILDDIR_CTYPES/jni"
    fail_panic "Can't create directory: $BUILDDIR_CTYPES/jni"

    run mkdir -p "$BUILDDIR_CTYPES/jni/include"
    fail_panic "Can't create directory: $BUILDDIR_CTYPES/jni/include"

    run cp -p $BUILDDIR_CTYPES_CONFIG/fficonfig.h $BUILDDIR_CTYPES_CONFIG/include/*.h $BUILDDIR_CTYPES/jni/include
    fail_panic "Can't copy configured libffi headers"

    local FFI_SRC_LIST
    case $ABI in
        x86)
            FFI_SRC_LIST="src/x86/ffi.c src/x86/sysv.S src/x86/win32.S"
            ;;
        x86_64)
            FFI_SRC_LIST="src/x86/ffi64.c src/x86/unix64.S"
            ;;
        armeabi*)
            FFI_SRC_LIST="src/arm/ffi.c src/arm/sysv.S"
            ;;
        arm64-v8a)
            FFI_SRC_LIST="src/aarch64/ffi.c src/aarch64/sysv.S"
            ;;
        mips)
            FFI_SRC_LIST="src/mips/ffi.c src/mips/o32.S"
            ;;
        mips64)
            FFI_SRC_LIST="src/mips/ffi.c src/mips/o32.S src/mips/n32.S"
            ;;
        *)
            panic "Unknown ABI: '$ABI'"
            ;;
    esac
    FFI_SRC_LIST="$FFI_SRC_LIST src/prep_cif.c"
    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := _ctypes'
        echo 'LOCAL_C_INCLUDES := $(LOCAL_PATH)/include'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        for ffi_src in $FFI_SRC_LIST; do
            echo "  \$(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/libffi/$ffi_src \\"
        done
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/callbacks.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/callproc.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/cfield.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/malloc_closure.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/stgdict.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ctypes/_ctypes.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_CTYPES/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_CTYPES/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_CTYPES -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module '_ctypes'"

    log "Install python$PYTHON_ABI-$ABI module '_ctypes' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_CTYPES/lib_ctypes.so $PYBIN_INSTALLDIR_MODULES/_ctypes.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module '_ctypes' in $PYBIN_INSTALLDIR_MODULES"

# _multiprocessing
    local BUILDDIR_MULTIPROCESSING="$BUILDDIR/multiprocessing"
    local OBJDIR_MULTIPROCESSING="$BUILDDIR_MULTIPROCESSING/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_MULTIPROCESSING/jni"
    fail_panic "Can't create directory: $BUILDDIR_MULTIPROCESSING/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := _multiprocessing'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        if [ "$PYTHON_MAJOR_VERSION" = "2" ]; then
            echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_multiprocessing/socket_connection.c \'
        fi
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_multiprocessing/multiprocessing.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_multiprocessing/semaphore.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_MULTIPROCESSING/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_MULTIPROCESSING/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_MULTIPROCESSING -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module '_multiprocessing'"

    log "Install python$PYTHON_ABI-$ABI module '_multiprocessing' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_MULTIPROCESSING/lib_multiprocessing.so $PYBIN_INSTALLDIR_MODULES/_multiprocessing.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module '_multiprocessing' in $PYBIN_INSTALLDIR_MODULES"

# _socket
    local BUILDDIR_SOCKET="$BUILDDIR/socket"
    local OBJDIR_SOCKET="$BUILDDIR_SOCKET/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_SOCKET/jni"
    fail_panic "Can't create directory: $BUILDDIR_SOCKET/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := _socket'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/socketmodule.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_SOCKET/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_SOCKET/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_SOCKET -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module '_socket'"

    log "Install python$PYTHON_ABI-$ABI module '_socket' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_SOCKET/lib_socket.so $PYBIN_INSTALLDIR_MODULES/_socket.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module '_socket' in $PYBIN_INSTALLDIR_MODULES"

#_ssl
    if [ -n "$OPENSSL_HOME" ]; then
        local BUILDDIR_SSL="$BUILDDIR/ssl"
        local OBJDIR_SSL="$BUILDDIR_SSL/obj/local/$ABI"

        run mkdir -p "$BUILDDIR_SSL/jni"
        fail_panic "Can't create directory: $BUILDDIR_SSL/jni"

        {
            echo 'LOCAL_PATH := $(call my-dir)'
            echo 'include $(CLEAR_VARS)'
            echo 'LOCAL_MODULE := _ssl'
            echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
            echo 'LOCAL_SRC_FILES := \'
            echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_ssl.c'
            echo 'LOCAL_STATIC_LIBRARIES := python_shared openssl_static opencrypto_static'
            echo 'include $(BUILD_SHARED_LIBRARY)'
            echo "\$(call import-module,python/$PYTHON_ABI)"
            echo "\$(call import-module,$OPENSSL_HOME)"
        } >$BUILDDIR_SSL/jni/Android.mk
        fail_panic "Can't generate $BUILDDIR_SSL/jni/Android.mk"

        run $NDK_DIR/ndk-build -C $BUILDDIR_SSL -j$NUM_JOBS APP_ABI=$ABI V=1
        fail_panic "Can't build python$PYTHON_ABI-$ABI module '_ssl'"

        log "Install python$PYTHON_ABI-$ABI module '_ssl' in $PYBIN_INSTALLDIR_MODULES"
        run cp -p -T $OBJDIR_SSL/lib_ssl.so $PYBIN_INSTALLDIR_MODULES/_ssl.so
        fail_panic "Can't install python$PYTHON_ABI-$ABI module '_ssl' in $PYBIN_INSTALLDIR_MODULES"
    fi

# _sqlite3
    local BUILDDIR_SQLITE3="$BUILDDIR/sqlite3"
    local OBJDIR_SQLITE3="$BUILDDIR_SQLITE3/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_SQLITE3/jni"
    fail_panic "Can't create directory: $BUILDDIR_SQLITE3/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := _sqlite3'
        echo 'LOCAL_CFLAGS := -DMODULE_NAME=\"sqlite3\"'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/cache.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/connection.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/cursor.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/microprotocols.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/module.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/prepare_protocol.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/row.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/statement.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/_sqlite/util.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared sqlite3_static'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
        echo '$(call import-module,sqlite/3)'
    } >$BUILDDIR_SQLITE3/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_SQLITE3/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_SQLITE3 -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module '_sqlite3'"

    log "Install python$PYTHON_ABI-$ABI module '_sqlite3' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_SQLITE3/lib_sqlite3.so $PYBIN_INSTALLDIR_MODULES/_sqlite3.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module '_sqlite3' in $PYBIN_INSTALLDIR_MODULES"

#pyexpat
    local BUILDDIR_PYEXPAT="$BUILDDIR/pyexpat"
    local OBJDIR_PYEXPAT="$BUILDDIR_PYEXPAT/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_PYEXPAT/jni"
    fail_panic "Can't create directory: $BUILDDIR_PYEXPAT/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := pyexpat'
        echo 'LOCAL_CFLAGS := -DHAVE_EXPAT_CONFIG_H -DXML_STATIC'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo "LOCAL_C_INCLUDES := \$(MY_PYTHON_SRC_ROOT)/Modules/expat"
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/expat/xmlparse.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/expat/xmlrole.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/expat/xmltok.c \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/pyexpat.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_PYEXPAT/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_PYEXPAT/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_PYEXPAT -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module 'pyexpat'"

    log "Install python$PYTHON_ABI-$ABI module 'pyexpat' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_PYEXPAT/libpyexpat.so $PYBIN_INSTALLDIR_MODULES/pyexpat.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module 'pyexpat' in $PYBIN_INSTALLDIR_MODULES"

# select
    local BUILDDIR_SELECT="$BUILDDIR/select"
    local OBJDIR_SELECT="$BUILDDIR_SELECT/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_SELECT/jni"
    fail_panic "Can't create directory: $BUILDDIR_SELECT/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := select'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/selectmodule.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_SELECT/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_SELECT/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_SELECT -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module 'select'"

    log "Install python$PYTHON_ABI-$ABI module 'select' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_SELECT/libselect.so $PYBIN_INSTALLDIR_MODULES/select.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module 'select' in $PYBIN_INSTALLDIR_MODULES"

# unicodedata
    local BUILDDIR_UNICODEDATA="$BUILDDIR/unicodedata"
    local OBJDIR_UNICODEDATA="$BUILDDIR_UNICODEDATA/obj/local/$ABI"

    run mkdir -p "$BUILDDIR_UNICODEDATA/jni"
    fail_panic "Can't create directory: $BUILDDIR_UNICODEDATA/jni"

    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := unicodedata'
        echo "MY_PYTHON_SRC_ROOT := $PYTHON_SRCDIR"
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_PYTHON_SRC_ROOT)/Modules/unicodedata.c'
        echo 'LOCAL_STATIC_LIBRARIES := python_shared'
        echo 'include $(BUILD_SHARED_LIBRARY)'
        echo "\$(call import-module,python/$PYTHON_ABI)"
    } >$BUILDDIR_UNICODEDATA/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_UNICODEDATA/jni/Android.mk"

    run $NDK_DIR/ndk-build -C $BUILDDIR_UNICODEDATA -j$NUM_JOBS APP_ABI=$ABI V=1
    fail_panic "Can't build python$PYTHON_ABI-$ABI module 'unicodedata'"

    log "Install python$PYTHON_ABI-$ABI module 'unicodedata' in $PYBIN_INSTALLDIR_MODULES"
    run cp -p -T $OBJDIR_UNICODEDATA/libunicodedata.so $PYBIN_INSTALLDIR_MODULES/unicodedata.so
    fail_panic "Can't install python$PYTHON_ABI-$ABI module 'unicodedata' in $PYBIN_INSTALLDIR_MODULES"
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
