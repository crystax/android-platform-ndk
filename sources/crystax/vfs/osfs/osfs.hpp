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

#ifndef _CRYSTAX_FILEIO_OSFS_HPP_f0c846419e2445799a2feab251ed1204
#define _CRYSTAX_FILEIO_OSFS_HPP_f0c846419e2445799a2feab251ed1204

#include "fileio/common.hpp"

/* Values for HyFileOpen */
#define HyMaxPath 1024
#define HyOpenRead    1
#define HyOpenWrite   2
#define HyOpenCreate  4
#define HyOpenTruncate  8
#define HyOpenAppend  16
#define HyOpenText    32
/* Use this flag with HyOpenCreate, if this flag is specified then
 * trying to create an existing file will fail
 */
#define HyOpenCreateNew 64
#define HyOpenSync      128
#define SHARED_LOCK_TYPE 1L

#ifdef TEMP_FAILURE_RETRY
#undef TEMP_FAILURE_RETRY
#endif
#define TEMP_FAILURE_RETRY(exp) ({         \
    typeof (exp) _rc;                      \
    do {                                   \
        _rc = (exp);                       \
        DBG("_rc=%d", (int)_rc);           \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })

#ifdef NATIVE_METHOD
#undef NATIVE_METHOD
#endif
#define NATIVE_METHOD(className, functionName, signature) \
    { #functionName, signature, reinterpret_cast<void*>(className ## _ ## functionName) }

namespace crystax
{
namespace fileio
{
namespace osfs
{

extern jni::jhclass clsFD;
extern jmethodID midFDCtor;
extern jfieldID fidFD;

void getExceptionSummary(JNIEnv* env, jthrowable exception, char* buf, size_t bufLen);
const char* jniStrError(int errnum, char* buf, size_t buflen);
int jniThrowException(JNIEnv* env, const char* className, const char* msg);
int jniThrowNullPointerException(JNIEnv* env, const char* msg);
int jniThrowRuntimeException(JNIEnv* env, const char* msg);
int jniThrowIOException(JNIEnv* env, int errnum);
int jniThrowExceptionFmt(JNIEnv* env, const char* className, const char* fmt, va_list args);

inline int jniThrowExceptionFmt(JNIEnv* env, const char* className, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    return jniThrowExceptionFmt(env, className, fmt, args);
    va_end(args);
}

jobject jniCreateFileDescriptor(JNIEnv* env, int fd);
int jniGetFDFromFileDescriptor(JNIEnv* env, jobject fileDescriptor);
void jniSetFileDescriptorOfFD(JNIEnv* env, jobject fileDescriptor, int value);

jlong translateLockLength(jlong length);
bool offsetTooLarge(JNIEnv* env, jlong longOffset);
struct flock flockFromStartAndLength(jlong start, jlong length);
int EsTranslateOpenFlags(int flags);

// ScopedBooleanArrayRO, ScopedByteArrayRO, ScopedCharArrayRO, ScopedDoubleArrayRO,
// ScopedFloatArrayRO, ScopedIntArrayRO, ScopedLongArrayRO, and ScopedShortArrayRO provide
// convenient read-only access to Java arrays from JNI code. This is cheaper than read-write
// access and should be used by default.
#define INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(PRIMITIVE_TYPE, NAME) \
    class Scoped ## NAME ## ArrayRO { \
    public: \
        Scoped ## NAME ## ArrayRO(JNIEnv* env, PRIMITIVE_TYPE ## Array javaArray) \
        : mEnv(env), mJavaArray(javaArray), mRawArray(NULL) { \
            if (mJavaArray == NULL) { \
                jniThrowNullPointerException(mEnv, NULL); \
            } else { \
                mRawArray = mEnv->Get ## NAME ## ArrayElements(mJavaArray, NULL); \
            } \
        } \
        ~Scoped ## NAME ## ArrayRO() { \
            if (mRawArray) { \
                mEnv->Release ## NAME ## ArrayElements(mJavaArray, mRawArray, JNI_ABORT); \
            } \
        } \
        const PRIMITIVE_TYPE* get() const { return mRawArray; } \
        const PRIMITIVE_TYPE& operator[](size_t n) const { return mRawArray[n]; } \
        size_t size() const { return mEnv->GetArrayLength(mJavaArray); } \
    private: \
        JNIEnv* mEnv; \
        PRIMITIVE_TYPE ## Array mJavaArray; \
        PRIMITIVE_TYPE* mRawArray; \
        Scoped ## NAME ## ArrayRO(const Scoped ## NAME ## ArrayRO&); \
        void operator=(const Scoped ## NAME ## ArrayRO&); \
    }

INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jboolean, Boolean);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jbyte, Byte);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jchar, Char);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jdouble, Double);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jfloat, Float);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jint, Int);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jlong, Long);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO(jshort, Short);

#undef INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RO

// ScopedBooleanArrayRW, ScopedByteArrayRW, ScopedCharArrayRW, ScopedDoubleArrayRW,
// ScopedFloatArrayRW, ScopedIntArrayRW, ScopedLongArrayRW, and ScopedShortArrayRW provide
// convenient read-write access to Java arrays from JNI code. These are more expensive,
// since they entail a copy back onto the Java heap, and should only be used when necessary.
#define INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(PRIMITIVE_TYPE, NAME) \
    class Scoped ## NAME ## ArrayRW { \
    public: \
        Scoped ## NAME ## ArrayRW(JNIEnv* env, PRIMITIVE_TYPE ## Array javaArray) \
        : mEnv(env), mJavaArray(javaArray), mRawArray(NULL) { \
            if (mJavaArray == NULL) { \
                jniThrowNullPointerException(mEnv, NULL); \
            } else { \
                mRawArray = mEnv->Get ## NAME ## ArrayElements(mJavaArray, NULL); \
            } \
        } \
        ~Scoped ## NAME ## ArrayRW() { \
            if (mRawArray) { \
                mEnv->Release ## NAME ## ArrayElements(mJavaArray, mRawArray, 0); \
            } \
        } \
        const PRIMITIVE_TYPE* get() const { return mRawArray; } \
        const PRIMITIVE_TYPE& operator[](size_t n) const { return mRawArray[n]; } \
        PRIMITIVE_TYPE* get() { return mRawArray; } \
        PRIMITIVE_TYPE& operator[](size_t n) { return mRawArray[n]; } \
        size_t size() const { return mEnv->GetArrayLength(mJavaArray); } \
    private: \
        JNIEnv* mEnv; \
        PRIMITIVE_TYPE ## Array mJavaArray; \
        PRIMITIVE_TYPE* mRawArray; \
        Scoped ## NAME ## ArrayRW(const Scoped ## NAME ## ArrayRW&); \
        void operator=(const Scoped ## NAME ## ArrayRW&); \
    }

INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jboolean, Boolean);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jbyte, Byte);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jchar, Char);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jdouble, Double);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jfloat, Float);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jint, Int);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jlong, Long);
INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW(jshort, Short);

#undef INSTANTIATE_SCOPED_PRIMITIVE_ARRAY_RW

bool register_native_methods(JNIEnv *env, const char *clsname, const JNINativeMethod *table, int size);

} // namespace osfs
} // namespace fileio
} // namespace crystax

#endif // _CRYSTAX_FILEIO_OSFS_HPP_f0c846419e2445799a2feab251ed1204
