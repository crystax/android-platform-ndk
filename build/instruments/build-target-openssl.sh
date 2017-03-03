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

# include common function and variable definitions
. `dirname $0`/prebuilt-common.sh

PROGRAM_PARAMETERS="<src-dir>"

PROGRAM_DESCRIPTION=\
"Rebuild OpenSSL libraries for the CrystaX NDK.

This requires a temporary NDK installation containing
toolchain binaries for all target architectures.

By default, this will try with the current NDK directory, unless
you use the --ndk-dir=<path> option.

The output will be placed in appropriate sub-directories of
<ndk>/$OPENSSL_SUBDIR, but you can override this with the --out-dir=<path>
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

register_try64_option
register_jobs_option

extract_parameters "$@"

OPENSSL_SRCDIR=$(echo $PARAMETERS | sed 1q)
if [ -z "$OPENSSL_SRCDIR" ]; then
    panic "Please provide the path to the OpenSSL source tree. See --help"
fi

if [ ! -d "$OPENSSL_SRCDIR" ]; then
    panic "No such directory: '$OPENSSL_SRCDIR'"
fi

OPENSSL_SRCDIR=$(cd $OPENSSL_SRCDIR && pwd)
OPENSSL_SRC_VERSION=\
$(cat $OPENSSL_SRCDIR/crypto/opensslv.h | sed -n 's/#[ \t]*define[ \t]*OPENSSL_VERSION_TEXT[ \t]*"OpenSSL[ \t]*\([0-9\.]*[A-Za-z]\?\)[A-Za-z0-9 \.]*"/\1/p')

if [ -z "$OPENSSL_SRC_VERSION" ]; then
    panic "Can't detect OpenSSL version."
fi

OPENSSL_DSTDIR=$NDK_DIR/$OPENSSL_SUBDIR/$OPENSSL_SRC_VERSION
mkdir -p $OPENSSL_DSTDIR
fail_panic "Can't create OpenSSL destination directory: $OPENSSL_DSTDIR"

ABIS=$(commas_to_spaces $ABIS)

if [ -z "$OPTION_BUILD_DIR" ]; then
    BUILD_DIR="$NDK_TMPDIR/build-openssl"
else
    eval BUILD_DIR=$OPTION_BUILD_DIR
fi

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
fail_panic "Can't create build directory: $BUILD_DIR"


# $1: ABI
# $2: build directory
build_openssl_for_abi ()
{
    dump "Building OpenSSL-$OPENSSL_SRC_VERSION for $ABI"

    local ABI="$1"
    local BUILDDIR="$2"

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
    esac

    local TOOLCHAIN
    case $ABI in
        armeabi*)
            TOOLCHAIN=arm-linux-androideabi
            ;;
        x86)
            TOOLCHAIN=x86
            OPENSSL_TARGET=linux-elf
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


    local LDFLAGS=""
    if [ "$ABI" = "armeabi-v7a-hard" ]; then
        LDFLAGS="$LDFLAGS -Wl,--no-warn-mismatch"
    fi

    local TCPREFIX=$NDK_DIR/toolchains/${TOOLCHAIN}-4.9/prebuilt/$HOST_TAG

    local BIN_WRAP_DIR="$BUILDDIR/bin-ndk"
    mkdir -p "$BIN_WRAP_DIR"
    fail_panic "Can't create directory: $BIN_WRAP_DIR"

    # gcc wrapper
    local MY_GCC="$BIN_WRAP_DIR/${HOST}-gcc"
    {
        echo '#!/bin/bash -e'
        echo "MY_CFLAGS=\"$CFLAGS\""
        echo "MY_LDFLAGS=\"$LDFLAGS -L$NDK_DIR/sources/crystax/libs/$ABI\""
        echo 'ARGS='
        echo 'BUILD_DSO=yes'
        echo 'for p in "$@"; do'
        echo '    ARG_IS_SONAME=no'
        echo '    ARG_IS_RPATH=no'
        echo '    case $p in'
        echo '        -c)'
        echo '            BUILD_DSO=no'
        echo '            ;;'
        echo '        -Wl,-rpath,*)'
        echo '            ARG_IS_RPATH=yes'
        echo '            ;;'
        echo '        -Wl,-soname=*)'
        echo '            ARG_IS_SONAME=yes'
        echo '            ;;'
        echo '    esac'
        echo '    if [ "$ARG_IS_RPATH" = "yes" ]; then'
        echo '        p='
        echo '    fi'
        echo '    if [ "$ARG_IS_SONAME" = "yes" ]; then'
        echo '        p=$(echo $p | sed -n "s/\(.*\.so\).*/\1/p")'
        echo '    fi'
        echo '    if [ -n "$p" ]; then'
        echo '        ARGS="$ARGS $p"'
        echo '    fi'
        echo 'done'
        echo 'if [ "$BUILD_DSO" = "yes" ]; then'
        echo '    ARGS="$ARGS $MY_LDFLAGS"'
        echo 'else'
        echo '    ARGS="$MY_CFLAGS $ARGS"'
        echo 'fi'
        echo "exec $TCPREFIX/bin/${HOST}-gcc --sysroot=$NDK_DIR/platforms/android-$APILEVEL/arch-$ARCH \$ARGS"
    } >$MY_GCC
    fail_panic "Can't create gcc wrapper"
    chmod +x $MY_GCC
    fail_panic "Can't chmod +x gcc wrapper"

    # script to init obj-tree
    local OBJTREE_WRAPPER=$BUILDDIR/mkobjtree.sh
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo "OPENSSL_SOURCE=\"$OPENSSL_SRCDIR\""
        echo 'mkdir -p $DIR_HERE/objtree'
        echo 'cd $DIR_HERE/objtree'
        echo 'echo "Preparing OpenSSL obj-tree in $(pwd) ..."'
        echo '(cd $OPENSSL_SOURCE; find . -type f) | while read F; do'
        echo '    mkdir -p $(dirname $F)'
        echo '    rm -f $F'
        echo '    if [ "$F" = "./crypto/opensslconf.h" ]; then'
        echo '        cp -fpH $OPENSSL_SOURCE/crypto/opensslconf.h ./crypto'
        echo '    elif [ "$F" = "./crypto/cryptlib.h" ]; then'
        echo '        cp -fpH $OPENSSL_SOURCE/crypto/cryptlib.h ./crypto'
        echo '    elif [ "$F" = "./Makefile.org" ]; then'
        echo '        cp -fpH $OPENSSL_SOURCE/Makefile.org .'
        echo '    else'
        echo '        ln -s $OPENSSL_SOURCE/$F $F'
        echo '    fi'
        echo 'done'
    } >$OBJTREE_WRAPPER
    fail_panic "Can't create OpenSSL objtree wrapper"
    chmod +x $OBJTREE_WRAPPER
    fail_panic "Can't chmod +x OpenSSL objtree wrapper"

    local OPENSSL_TARGET
    case $ABI in
        x86)
            OPENSSL_TARGET=linux-elf
            ;;
        x86_64)
            OPENSSL_TARGET=linux-x86_64
            ;;
        armeabi*)
            OPENSSL_TARGET=linux-armv4
            ;;
        arm64-v8a)
            OPENSSL_TARGET=linux-aarch64
            ;;
        mips)
            # Looks like asm code in OpenSSL doesn't support MIPS32r6
            OPENSSL_TARGET=linux-generic32
            ;;
        mips64)
            # Looks like asm code in OpenSSL doesn't support MIPS64r6
            OPENSSL_TARGET=linux-generic64
            ;;
        *)
            panic "ERROR: Unknown ABI: '$ABI'"
    esac

    local OPENSSL_OPTIONS='shared zlib-dynamic -DOPENSSL_NO_DEPRECATED'

    # script for build
    local BUILD_WRAPPER=$BUILDDIR/build.sh
    {
        echo '#!/bin/bash -e'
        echo 'DIR_HERE=$(cd $(dirname $0) && pwd)'
        echo "export PATH=\"$BIN_WRAP_DIR:$TCPREFIX/bin:\$PATH\""
        echo 'cd $DIR_HERE'
        echo './mkobjtree.sh'
        echo 'cd objtree'
        echo "perl -p -i -e 's/^(install:.*)\\binstall_docs\\b(.*)$/\$1 \$2/g' Makefile.org"
        echo "perl ./Configure --openssldir=/system/etc/security --prefix=/pkg --cross-compile-prefix=\"${HOST}-\" $OPENSSL_OPTIONS $OPENSSL_TARGET"
        echo "perl -p -i -e 's/^(#\\s*define\\s+ENGINESDIR\\s+).*$/\$1NULL/g' crypto/opensslconf.h"
        echo "perl -p -i -e 's/^(#\\s*define\\s+X509_CERT_DIR\\s+OPENSSLDIR\\s+).*$/\$1\"\\/cacerts\"/g' crypto/cryptlib.h"
        echo "make -j$NUM_JOBS"
        echo "make INSTALL_PREFIX=$BUILDDIR/install install"
    } >$BUILD_WRAPPER
    fail_panic "Can't create OpenSSL build wrapper"
    chmod +x $BUILD_WRAPPER
    fail_panic "Can't chmod +x OpenSSL build wrapper"

    run $BUILD_WRAPPER
    fail_panic "Can't make OpenSSL-$OPENSSL_SRC_VERSION for $ABI"

    local OPENSSL_ABI_CONFIG_MAKE="$BUILDDIR/install/pkg/include/openssl/opensslconf.h"
    local OPENSSL_ABI_CONFIG="$BUILDDIR/install/pkg/include/openssl/opensslconf_$(echo $ABI | tr '-' '_').h"
    mv $OPENSSL_ABI_CONFIG_MAKE $OPENSSL_ABI_CONFIG
    fail_panic "Can't rename $OPENSSL_ABI_CONFIG_MAKE in $OPENSSL_ABI_CONFIG"

    local OPENSSL_HEADERS_DSTDIR="$OPENSSL_DSTDIR/include/openssl"
    if [ "$OPENSSL_HEADERS_INSTALLED" != "yes" ]; then
        log "Install OpenSSL headers into $OPENSSL_HEADERS_DSTDIR"
        run rm -Rf $OPENSSL_HEADERS_DSTDIR && run mkdir -p $OPENSSL_HEADERS_DSTDIR
        fail_panic "Can't create directory: $OPENSSL_HEADERS_DSTDIR"
        {
            echo '#if defined(__ARM_ARCH_5TE__)'
            echo '#include "opensslconf_armeabi.h"'
            echo '#elif defined(__ARM_ARCH_7A__) && !defined(__ARM_PCS_VFP)'
            echo '#include "opensslconf_armeabi_v7a.h"'
            echo '#elif defined(__ARM_ARCH_7A__) && defined(__ARM_PCS_VFP)'
            echo '#include "opensslconf_armeabi_v7a_hard.h"'
            echo '#elif defined(__aarch64__)'
            echo '#include "opensslconf_arm64_v8a.h"'
            echo '#elif defined(__i386__)'
            echo '#include "opensslconf_x86.h"'
            echo '#elif defined(__x86_64__)'
            echo '#include "opensslconf_x86_64.h"'
            echo '#elif defined(__mips__) && !defined(__mips64)'
            echo '#include "opensslconf_mips.h"'
            echo '#elif defined(__mips__) && defined(__mips64)'
            echo '#include "opensslconf_mips64.h"'
            echo '#else'
            echo '#error "Unsupported ABI"'
            echo '#endif'
        } >"$OPENSSL_HEADERS_DSTDIR/opensslconf.h"
        fail_panic "Can't generate $OPENSSL_HEADERS_DSTDIR/opensslconf.h"
        run cp -p $BUILDDIR/install/pkg/include/openssl/*.h $OPENSSL_HEADERS_DSTDIR
        fail_panic "Can't install OpenSSL headers"
        OPENSSL_HEADERS_INSTALLED=yes
        export OPENSSL_HEADERS_INSTALLED
    else
        run cp -p $OPENSSL_ABI_CONFIG $OPENSSL_HEADERS_DSTDIR
        fail_panic "Can't install $OPENSSL_ABI_CONFIG"
    fi

    log "Install OpenSSL binaries into $OPENSSL_DSTDIR"
    run rm -Rf $OPENSSL_DSTDIR/libs/$ABI && \
    run mkdir -p $OPENSSL_DSTDIR/libs/$ABI && \
    run cp -p \
      $BUILDDIR/install/pkg/lib/libcrypto.a \
      $BUILDDIR/install/pkg/lib/libcrypto.so \
      $BUILDDIR/install/pkg/lib/libssl.a \
      $BUILDDIR/install/pkg/lib/libssl.so \
      $OPENSSL_DSTDIR/libs/$ABI && \
    run chmod 0644 $OPENSSL_DSTDIR/libs/$ABI/lib*
    fail_panic "Can't install OpenSSL binaries"

    log "Build openssl tool for $ABI"
    local BUILDDIR_OPENSSL_TOOL="$BUILDDIR/tool"
    local OBJDIR_OPENSSL_TOOL="$BUILDDIR_OPENSSL_TOOL/obj/local/$ABI"
    run mkdir -p $BUILDDIR_OPENSSL_TOOL/jni
    fail_panic "Can't create directory: $BUILDDIR_OPENSSL_TOOL/jni"
    {
        echo 'LOCAL_PATH := $(call my-dir)'
        echo 'include $(CLEAR_VARS)'
        echo 'LOCAL_MODULE := openssl'
        echo "MY_OPENSSL_SRC_ROOT := $OPENSSL_SRCDIR"
        echo 'LOCAL_CFLAGS := -DMONOLITH -DOPENSSL_NO_DEPRECATED'
        echo 'LOCAL_C_INCLUDES := $(MY_OPENSSL_SRC_ROOT)'
        echo 'LOCAL_SRC_FILES := \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/app_rand.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/apps.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/asn1pars.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ca.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ciphers.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/cms.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/crl.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/crl2p7.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/dgst.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/dh.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/dhparam.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/dsa.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/dsaparam.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ec.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ecparam.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/enc.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/engine.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/errstr.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/gendh.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/gendsa.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/genpkey.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/genrsa.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/nseq.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ocsp.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/openssl.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/passwd.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkcs12.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkcs7.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkcs8.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkey.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkeyparam.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/pkeyutl.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/prime.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/rand.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/req.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/rsa.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/rsautl.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/s_cb.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/s_client.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/s_server.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/s_socket.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/s_time.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/sess_id.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/smime.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/speed.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/spkac.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/srp.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/ts.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/verify.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/version.c \'
        echo '  $(MY_OPENSSL_SRC_ROOT)/apps/x509.c'
        echo ''
        echo 'LOCAL_STATIC_LIBRARIES := openssl_static opencrypto_static'
        echo 'include $(BUILD_EXECUTABLE)'
        echo "\$(call import-module,openssl/$OPENSSL_SRC_VERSION)"
    } >$BUILDDIR_OPENSSL_TOOL/jni/Android.mk
    fail_panic "Can't generate $BUILDDIR_OPENSSL_TOOL/jni/Android.mk"

    local EXE_PLATFORM
    case $ABI in
        x86|armeabi*|mips)
            EXE_PLATFORM=android-16
            ;;
        x86_64|arm64-v8a|mips64)
            EXE_PLATFORM=android-21
            ;;
        *)
            panic "ERROR: Unknown ABI: '$ABI'"
    esac

    run $NDK_DIR/ndk-build -C $BUILDDIR_OPENSSL_TOOL -j$NUM_JOBS APP_ABI=$ABI V=1 APP_PLATFORM=$EXE_PLATFORM APP_PIE=true
    fail_panic "Can't build build openssl tool for $ABI"

    local OPENSSL_INSTALLDIR_BIN="$OPENSSL_DSTDIR/bin/$ABI"
    run mkdir -p $OPENSSL_INSTALLDIR_BIN
    fail_panic "Can't create directory: $OPENSSL_INSTALLDIR_BIN"

    log "Install openssl tool in $OPENSSL_INSTALLDIR_BIN"
    run cp -p -T "$OBJDIR_OPENSSL_TOOL/openssl" "$OPENSSL_INSTALLDIR_BIN/openssl"
    fail_panic "Can't install openssl tool in $OPENSSL_INSTALLDIR_BIN"
}


if [ -n "$PACKAGE_DIR" ]; then
    PACKAGE_NAME="openssl-${OPENSSL_SRC_VERSION}-headers.tar.xz"
    echo "Look for: $PACKAGE_NAME"
    try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
    if [ $? -eq 0 ]; then
        OPENSSL_HEADERS_NEED_PACKAGE=no
    else
        OPENSSL_HEADERS_NEED_PACKAGE=yes
    fi
fi

BUILT_ABIS=""
for ABI in $ABIS; do
    DO_BUILD_PACKAGE="yes"
    if [ -n "$PACKAGE_DIR" ]; then
        PACKAGE_NAME="openssl-${OPENSSL_SRC_VERSION}-binaries-${ABI}.tar.xz"
        echo "Look for: $PACKAGE_NAME"
        try_cached_package "$PACKAGE_DIR" "$PACKAGE_NAME" no_exit
        if [ $? -eq 0 ]; then
            if [ "$OPENSSL_HEADERS_NEED_PACKAGE" = "yes" -a -z "$BUILT_ABIS" ]; then
                BUILT_ABIS="$BUILT_ABIS $ABI"
            else
                DO_BUILD_PACKAGE="no"
            fi
        else
            BUILT_ABIS="$BUILT_ABIS $ABI"
        fi
    fi
    if [ "$DO_BUILD_PACKAGE" = "yes" ]; then
        build_openssl_for_abi $ABI "$BUILD_DIR/$ABI"
    fi
done

if [ -n "$PACKAGE_DIR" ]; then
    if [ "$OPENSSL_HEADERS_NEED_PACKAGE" = "yes" ]; then
        FILES="$OPENSSL_SUBDIR/${OPENSSL_SRC_VERSION}/include"
        PACKAGE_NAME="openssl-${OPENSSL_SRC_VERSION}-headers.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package openssl headers"
        cache_package "$PACKAGE_DIR" "$PACKAGE_NAME"
    fi

    for ABI in $BUILT_ABIS; do
        FILES=""
        for SUBDIR in libs/$ABI bin/$ABI; do
            FILES="$FILES $OPENSSL_SUBDIR/${OPENSSL_SRC_VERSION}/$SUBDIR"
        done
        PACKAGE_NAME="openssl-${OPENSSL_SRC_VERSION}-binaries-${ABI}.tar.xz"
        PACKAGE="$PACKAGE_DIR/$PACKAGE_NAME"
        dump "Packaging: $PACKAGE"
        pack_archive "$PACKAGE" "$NDK_DIR" "$FILES"
        fail_panic "Can't package openssl $ABI binaries"
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
