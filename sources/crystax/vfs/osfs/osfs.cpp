/*
 * Copyright (c) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Dmitry Moskalchuk.
 */

#include "osfs/osfs.hpp"

namespace crystax
{
namespace fileio
{
namespace osfs
{

namespace eclair
{
bool init(JNIEnv *env);
} // namespace eclair

namespace froyo
{
bool init(JNIEnv *env);
} // namespace froyo

namespace gingerbread
{
bool init(JNIEnv *env);
} // namespace gingerbread

namespace ics
{
bool init(JNIEnv *env);
} // namespace ics

using jni::jhclass;
using jni::jnienv;
using jni::jcast;
using jni::find_class;
using jni::get_field_id;
using jni::get_field;
using jni::get_static_field;

jhclass clsFD;
jmethodID midFDCtor;
jfieldID fidFD;

enum
{
    ANDROID_VERSION_BASE = 1,
    ANDROID_VERSION_BASE_1_1 = 2,
    ANDROID_VERSION_CUPCAKE = 3,
    ANDROID_VERSION_DONUT = 4,
    ANDROID_VERSION_ECLAIR = 5,
    ANDROID_VERSION_ECLAIR_0_1 = 6,
    ANDROID_VERSION_ECLAIR_MR1 = 7,
    ANDROID_VERSION_FROYO = 8,
    ANDROID_VERSION_GINGERBREAD = 9,
    ANDROID_VERSION_GINGERBREAD_MR1 = 10,
    ANDROID_VERSION_HONEYCOMB = 11,
    ANDROID_VERSION_HONEYCOMB_MR1 = 12,
    ANDROID_VERSION_HONEYCOMB_MR2 = 13,
    ANDROID_VERSION_ICE_CREAM_SANDWICH = 14,
    ANDROID_VERSION_ICE_CREAM_SANDWICH_MR1 = 15
};

/*
 * Get a human-readable summary of an exception object.  The buffer will
 * be populated with the "binary" class name and, if present, the
 * exception message.
 */
CRYSTAX_LOCAL
void getExceptionSummary(JNIEnv* env, jthrowable exception, char* buf, size_t bufLen)
{
    TRACE;
    int success = 0;

    /* get the name of the exception's class */
    jclass exceptionClazz = env->GetObjectClass(exception); // can't fail
    jclass classClazz = env->GetObjectClass(exceptionClazz); // java.lang.Class, can't fail
    jmethodID classGetNameMethod = env->GetMethodID(
            classClazz, "getName", "()Ljava/lang/String;");
    jstring classNameStr = (jstring)env->CallObjectMethod(exceptionClazz, classGetNameMethod);
    if (classNameStr != NULL) {
        /* get printable string */
        const char* classNameChars = env->GetStringUTFChars(classNameStr, NULL);
        if (classNameChars != NULL) {
            /* if the exception has a message string, get that */
            jmethodID throwableGetMessageMethod = env->GetMethodID(
                    exceptionClazz, "getMessage", "()Ljava/lang/String;");
            jstring messageStr = (jstring)env->CallObjectMethod(
                    exception, throwableGetMessageMethod);

            if (messageStr != NULL) {
                const char* messageChars = env->GetStringUTFChars(messageStr, NULL);
                if (messageChars != NULL) {
                    snprintf(buf, bufLen, "%s: %s", classNameChars, messageChars);
                    env->ReleaseStringUTFChars(messageStr, messageChars);
                } else {
                    env->ExceptionClear(); // clear OOM
                    snprintf(buf, bufLen, "%s: <error getting message>", classNameChars);
                }
                env->DeleteLocalRef(messageStr);
            } else {
                strncpy(buf, classNameChars, bufLen);
                buf[bufLen - 1] = '\0';
            }

            env->ReleaseStringUTFChars(classNameStr, classNameChars);
            success = 1;
        }
        env->DeleteLocalRef(classNameStr);
    }
    env->DeleteLocalRef(classClazz);
    env->DeleteLocalRef(exceptionClazz);

    if (! success) {
        env->ExceptionClear();
        snprintf(buf, bufLen, "%s", "<error getting class name>");
    }
}

CRYSTAX_LOCAL
const char* jniStrError(int errnum, char* buf, size_t buflen)
{
    TRACE;
    // note: glibc has a nonstandard strerror_r that returns char* rather
    // than POSIX's int.
    // char *strerror_r(int errnum, char *buf, size_t n);
    char* ret = (char*) strerror_r(errnum, buf, buflen);
    if (((int)ret) == 0) {
        //POSIX strerror_r, success
        return buf;
    } else if (((int)ret) == -1) {
        //POSIX strerror_r, failure
        // (Strictly, POSIX only guarantees a value other than 0. The safest
        // way to implement this function is to use C++ and overload on the
        // type of strerror_r to accurately distinguish GNU from POSIX. But
        // realistic implementations will always return -1.)
        snprintf(buf, buflen, "errno %d", errnum);
        return buf;
    } else {
        //glibc strerror_r returning a string
        return ret;
    }
}

/*
 * Throw an exception with the specified class and an optional message.
 *
 * If an exception is currently pending, we log a warning message and
 * clear it.
 *
 * Returns 0 if the specified exception was successfully thrown.  (Some
 * sort of exception will always be pending when this returns.)
 */
CRYSTAX_LOCAL
int jniThrowException(JNIEnv* env, const char* className, const char* msg)
{
    TRACE;
    jclass exceptionClass;

    if (env->ExceptionCheck()) {
        /* TODO: consider creating the new exception with this as "cause" */
        char buf[256];

        jthrowable exception = env->ExceptionOccurred();
        env->ExceptionClear();

        if (exception != NULL) {
            getExceptionSummary(env, exception, buf, sizeof(buf));
            env->DeleteLocalRef(exception);
        }
    }

    exceptionClass = env->FindClass(className);
    if (exceptionClass == NULL) {
        /* ClassNotFoundException now pending */
        return -1;
    }

    int result = 0;
    if (env->ThrowNew(exceptionClass, msg) != JNI_OK) {
        /* an exception, most likely OOM, will now be pending */
        result = -1;
    }

    env->DeleteLocalRef(exceptionClass);
    return result;
}

/*
 * Throw a java.lang.NullPointerException, with an optional message.
 */
CRYSTAX_LOCAL
int jniThrowNullPointerException(JNIEnv* env, const char* msg)
{
    TRACE;
    return jniThrowException(env, "java/lang/NullPointerException", msg);
}

/*
 * Throw a java.lang.RuntimeException, with an optional message.
 */
CRYSTAX_LOCAL
int jniThrowRuntimeException(JNIEnv* env, const char* msg)
{
    TRACE;
    return jniThrowException(env, "java/lang/RuntimeException", msg);
}

/*
 * Throw a java.io.IOException, generating the message from errno.
 */
CRYSTAX_LOCAL
int jniThrowIOException(JNIEnv* env, int errnum)
{
    TRACE;
    char buffer[80];
    const char* message = jniStrError(errnum, buffer, sizeof(buffer));
    return jniThrowException(env, "java/io/IOException", message);
}

CRYSTAX_LOCAL
int jniThrowExceptionFmt(JNIEnv* env, const char* className, const char* fmt, va_list args) {
    char msgBuf[512];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    return jniThrowException(env, className, msgBuf);
}

CRYSTAX_LOCAL
jobject jniCreateFileDescriptor(JNIEnv* env, int fd) {
    DBG("fd=%d", fd);
    jobject fileDescriptor = env->NewObject(clsFD.get(), midFDCtor);
    jniSetFileDescriptorOfFD(env, fileDescriptor, fd);
    return fileDescriptor;
}

CRYSTAX_LOCAL
int jniGetFDFromFileDescriptor(JNIEnv* env, jobject fileDescriptor) {
    TRACE;
    return env->GetIntField(fileDescriptor, fidFD);
}

CRYSTAX_LOCAL
void jniSetFileDescriptorOfFD(JNIEnv* env, jobject fileDescriptor, int fd) {
    DBG("fd=%d", fd);
    env->SetIntField(fileDescriptor, fidFD, fd);
}

CRYSTAX_LOCAL
jlong translateLockLength(jlong length) {
    TRACE;
    // FileChannel.tryLock uses Long.MAX_VALUE to mean "lock the whole
    // file", where POSIX would use 0. We can support that special case,
    // even for files whose actual length we can't represent. For other
    // out of range lengths, though, we want our range checking to fire.
    return (length == 0x7fffffffffffffffLL) ? 0 : length;
}

// Checks whether we can safely treat the given jlong as an off_t without
// accidental loss of precision.
// TODO: this is bogus; we should use _FILE_OFFSET_BITS=64.
CRYSTAX_LOCAL
bool offsetTooLarge(JNIEnv* env, jlong longOffset) {
    TRACE;
    if (sizeof(off_t) >= sizeof(jlong)) {
        // We're only concerned about the possibility that off_t is
        // smaller than jlong. off_t is signed, so we don't need to
        // worry about signed/unsigned.
        return false;
    }

    // TODO: use std::numeric_limits<off_t>::max() and min() when we have them.
    assert(sizeof(off_t) == sizeof(int));
    static const off_t off_t_max = INT_MAX;
    static const off_t off_t_min = INT_MIN;

    if (longOffset > off_t_max || longOffset < off_t_min) {
        // "Value too large for defined data type".
        jniThrowIOException(env, EOVERFLOW);
        return true;
    }
    return false;
}

CRYSTAX_LOCAL
struct flock flockFromStartAndLength(jlong start, jlong length) {
    TRACE;
    struct flock lock;
    memset(&lock, 0, sizeof(lock));

    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = length;

    return lock;
}

CRYSTAX_LOCAL
int EsTranslateOpenFlags(int flags) {
    TRACE;
    int realFlags = 0;

    if (flags & HyOpenAppend) {
        realFlags |= O_APPEND;
    }
    if (flags & HyOpenTruncate) {
        realFlags |= O_TRUNC;
    }
    if (flags & HyOpenCreate) {
        realFlags |= O_CREAT;
    }
    if (flags & HyOpenCreateNew) {
        realFlags |= O_EXCL | O_CREAT;
    }
#ifdef O_SYNC
    if (flags & HyOpenSync) {
        realFlags |= O_SYNC;
    }
#endif
    if (flags & HyOpenRead) {
        if (flags & HyOpenWrite) {
            return (O_RDWR | realFlags);
        }
        return (O_RDONLY | realFlags);
    }
    if (flags & HyOpenWrite) {
        return (O_WRONLY | realFlags);
    }
    return -1;
}

CRYSTAX_LOCAL
bool register_native_methods(JNIEnv *env, const char *clsname, const JNINativeMethod *table, int size)
{
    DBG("Registering %s natives", clsname);

    jclass cls = env->FindClass(clsname);
    if (!cls)
    {
        ERR("Native registration unable to find class %s", clsname);
        return false;
    }

    bool result = env->RegisterNatives(cls, table, size) == 0;
    if (!result)
        ERR("RegisterNatives failed for %s", clsname);

    env->DeleteLocalRef(cls);
    return result;
}

CRYSTAX_LOCAL
bool init(JNIEnv *env)
{
    TRACE;

    DBG("init FileDescriptor native variables");
    clsFD = find_class(env, "java/io/FileDescriptor");
    midFDCtor = get_method_id(env, clsFD, "<init>", "()V");
    fidFD = get_field_id(env, clsFD, "descriptor", "I");

    DBG("detect API level...");
    jint sdkInt = get_static_field<jint>(env, "android/os/Build$VERSION", "SDK_INT");
    DBG("sdkInt: %d", (int)sdkInt);

    switch (sdkInt)
    {
    case ANDROID_VERSION_BASE:
    case ANDROID_VERSION_BASE_1_1:
    case ANDROID_VERSION_CUPCAKE:
    case ANDROID_VERSION_DONUT:
        ERR("not supported android version: %d", (int)sdkInt);
        return false;
    case ANDROID_VERSION_ECLAIR:
    case ANDROID_VERSION_ECLAIR_0_1:
    case ANDROID_VERSION_ECLAIR_MR1:
        DBG("use eclair implementation");
        return eclair::init(env);
    case ANDROID_VERSION_FROYO:
        DBG("use froyo implementation");
        return froyo::init(env);
    case ANDROID_VERSION_GINGERBREAD:
    case ANDROID_VERSION_GINGERBREAD_MR1:
    case ANDROID_VERSION_HONEYCOMB:
    case ANDROID_VERSION_HONEYCOMB_MR1:
    case ANDROID_VERSION_HONEYCOMB_MR2:
        DBG("use gingerbread implementation");
        return gingerbread::init(env);
    default:
        DBG("use ics implementation");
        return ics::init(env);
    }
}

} // namespace osfs
} // namespace fileio
} // namespace crystax
