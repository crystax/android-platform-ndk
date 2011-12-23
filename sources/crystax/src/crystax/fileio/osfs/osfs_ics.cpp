/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

#include "fileio/osfs/osfs.hpp"
#include "fileio/osfs/asyncsocketclosemonitor.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <sys/vfs.h> // Bionic doesn't have <sys/statvfs.h>
#include <sys/wait.h>
#include <unistd.h>

#define NET_FAILURE_RETRY(syscall_name, exp) ({ \
    typeof (exp) _rc = -1; \
    do { \
        { \
            fd = jniGetFDFromFileDescriptor(env, javaFd); \
            DBG("fd=%d", (int)fd); \
            AsynchronousSocketCloseMonitor monitor(fd); \
            _rc = (exp); \
        } \
        if (_rc == -1) { \
            if (jniGetFDFromFileDescriptor(env, javaFd) == -1) { \
                jniThrowException(env, "java/net/SocketException", "Socket closed"); \
                break; \
            } else if (errno != EINTR) { \
                throwErrnoException(env, syscall_name); \
                break; \
            } \
        } \
    } while (_rc == -1); \
    _rc; })

#define TO_JAVA_STRING(NAME, EXP) \
        jstring NAME = env->NewStringUTF(EXP); \
        if (NAME == NULL) return NULL;

namespace crystax
{
namespace fileio
{
namespace osfs
{
namespace ics
{

static jni::jhclass byteArrayClass;
static jni::jhclass errnoExceptionClass;
static jni::jhclass gaiExceptionClass;
static jni::jhclass inet6AddressClass;
static jni::jhclass inetAddressClass;
static jni::jhclass inetSocketAddressClass;
static jni::jhclass mutableIntClass;
static jni::jhclass mutableLongClass;
static jni::jhclass stringClass;
static jni::jhclass structAddrinfoClass;
static jni::jhclass structFlockClass;
static jni::jhclass structGroupReqClass;
static jni::jhclass structLingerClass;
static jni::jhclass structPasswdClass;
static jni::jhclass structPollfdClass;
static jni::jhclass structStatClass;
static jni::jhclass structStatFsClass;
static jni::jhclass structTimevalClass;
static jni::jhclass structUtsnameClass;

static void throwException(JNIEnv* env, jclass exceptionClass, jmethodID ctor3, jmethodID ctor2,
        const char* functionName, int error) {
    TRACE;
    jthrowable cause = NULL;
    if (env->ExceptionCheck()) {
        cause = env->ExceptionOccurred();
        env->ExceptionClear();
    }

    jni::jhstring detailMessage(env->NewStringUTF(functionName));
    if (detailMessage.get() == NULL) {
        // Not really much we can do here. We're probably dead in the water,
        // but let's try to stumble on...
        env->ExceptionClear();
    }

    jobject exception;
    if (cause != NULL) {
        exception = env->NewObject(exceptionClass, ctor3, detailMessage.get(), error, cause);
    } else {
        exception = env->NewObject(exceptionClass, ctor2, detailMessage.get(), error);
    }
    env->Throw(reinterpret_cast<jthrowable>(exception));
}

static void throwErrnoException(JNIEnv* env, const char* functionName) {
    TRACE;
    int error = errno;
    static jmethodID ctor3 = env->GetMethodID(errnoExceptionClass.get(),
            "<init>", "(Ljava/lang/String;ILjava/lang/Throwable;)V");
    static jmethodID ctor2 = env->GetMethodID(errnoExceptionClass.get(),
            "<init>", "(Ljava/lang/String;I)V");
    throwException(env, errnoExceptionClass.get(), ctor3, ctor2, functionName, error);
}

static void throwGaiException(JNIEnv* env, const char* functionName, int error) {
    TRACE;
    if (error == EAI_SYSTEM) {
        // EAI_SYSTEM means "look at errno instead", so we want our GaiException to have the
        // relevant ErrnoException as its cause.
        throwErrnoException(env, functionName);
        // Deliberately fall through to throw another exception...
    }
    static jmethodID ctor3 = env->GetMethodID(gaiExceptionClass.get(),
            "<init>", "(Ljava/lang/String;ILjava/lang/Throwable;)V");
    static jmethodID ctor2 = env->GetMethodID(gaiExceptionClass.get(),
            "<init>", "(Ljava/lang/String;I)V");
    throwException(env, gaiExceptionClass.get(), ctor3, ctor2, functionName, error);
}

template <typename Counter, typename Getter>
jobjectArray toStringArray(JNIEnv* env, Counter* counter, Getter* getter) {
    size_t count = (*counter)();
    jobjectArray result = env->NewObjectArray(count, stringClass.get(), NULL);
    if (result == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        jni::jhstring s(env->NewStringUTF((*getter)(i)));
        if (env->ExceptionCheck()) {
            return NULL;
        }
        env->SetObjectArrayElement(result, i, s.get());
        if (env->ExceptionCheck()) {
            return NULL;
        }
    }
    return result;
}

struct ArrayCounter {
    const char* const* strings;
    ArrayCounter(const char* const* strings) : strings(strings) {}
    size_t operator()() {
        size_t count = 0;
        while (strings[count] != NULL) {
            ++count;
        }
        return count;
    }
};

struct ArrayGetter {
    const char* const* strings;
    ArrayGetter(const char* const* strings) : strings(strings) {}
    const char* operator()(size_t i) {
        return strings[i];
    }
};

jobjectArray toStringArray(JNIEnv* env, const char* const* strings) {
    ArrayCounter counter(strings);
    ArrayGetter getter(strings);
    return toStringArray(env, &counter, &getter);
}

template <typename rc_t>
static rc_t throwIfMinusOne(JNIEnv* env, const char* name, rc_t rc) {
    DBG("rc=%d", (int)rc);
    if (rc == rc_t(-1)) {
        throwErrnoException(env, name);
    }
    return rc;
}

class ScopedUtfChars {
public:
    ScopedUtfChars(JNIEnv* env, jstring s)
    : mEnv(env), mString(s)
    {
        if (s == NULL) {
            mUtfChars = NULL;
            jniThrowNullPointerException(env, NULL);
        } else {
            mUtfChars = env->GetStringUTFChars(s, NULL);
        }
    }

    ~ScopedUtfChars() {
        if (mUtfChars) {
            mEnv->ReleaseStringUTFChars(mString, mUtfChars);
        }
    }

    const char* c_str() const {
        return mUtfChars;
    }

    size_t size() const {
        return strlen(mUtfChars);
    }

    // Element access.
    const char& operator[](size_t n) const {
        return mUtfChars[n];
    }

private:
    JNIEnv* mEnv;
    jstring mString;
    const char* mUtfChars;

    // Disallow copy and assignment.
    ScopedUtfChars(const ScopedUtfChars&);
    void operator=(const ScopedUtfChars&);
};

template<bool readOnly>
class ScopedBytes {
public:
    ScopedBytes(JNIEnv* env, jobject object)
    : mEnv(env), mObject(object), mByteArray(NULL), mPtr(NULL)
    {
        if (mObject == NULL) {
            jniThrowNullPointerException(mEnv, NULL);
        } else if (mEnv->IsInstanceOf(mObject, byteArrayClass.get())) {
            mByteArray = reinterpret_cast<jbyteArray>(mObject);
            mPtr = mEnv->GetByteArrayElements(mByteArray, NULL);
        } else {
            mPtr = reinterpret_cast<jbyte*>(mEnv->GetDirectBufferAddress(mObject));
        }
    }

    ~ScopedBytes() {
        if (mByteArray != NULL) {
            mEnv->ReleaseByteArrayElements(mByteArray, mPtr, readOnly ? JNI_ABORT : 0);
        }
    }

private:
    JNIEnv* mEnv;
    jobject mObject;
    jbyteArray mByteArray;

protected:
    jbyte* mPtr;

private:
    // Disallow copy and assignment.
    ScopedBytes(const ScopedBytes&);
    void operator=(const ScopedBytes&);
};

class ScopedBytesRO : public ScopedBytes<true> {
public:
    ScopedBytesRO(JNIEnv* env, jobject object) : ScopedBytes<true>(env, object) {}
    const jbyte* get() const {
        return mPtr;
    }
};

class ScopedBytesRW : public ScopedBytes<false> {
public:
    ScopedBytesRW(JNIEnv* env, jobject object) : ScopedBytes<false>(env, object) {}
    jbyte* get() {
        return mPtr;
    }
};

template <typename T>
struct DefaultDelete {
    enum { type_must_be_complete = sizeof(T) };
    DefaultDelete() {}
    void operator()(T* p) const {
        delete p;
    }
};

// Default deleter for array types.
template <typename T>
struct DefaultDelete<T[]> {
    enum { type_must_be_complete = sizeof(T) };
    void operator()(T* p) const {
        delete[] p;
    }
};

// A smart pointer that deletes the given pointer on destruction.
// Equivalent to C++0x's std::unique_ptr (a combination of boost::scoped_ptr
// and boost::scoped_array).
// Named to be in keeping with Android style but also to avoid
// collision with any other implementation, until we can switch over
// to unique_ptr.
// Use thus:
//   UniquePtr<C> c(new C);
template <typename T, typename D = DefaultDelete<T> >
class UniquePtr {
public:
    // Construct a new UniquePtr, taking ownership of the given raw pointer.
    explicit UniquePtr(T* ptr = NULL) : mPtr(ptr) {
    }

    ~UniquePtr() {
        reset();
    }

    // Accessors.
    T& operator*() const { return *mPtr; }
    T* operator->() const { return mPtr; }
    T* get() const { return mPtr; }

    // Returns the raw pointer and hands over ownership to the caller.
    // The pointer will not be deleted by UniquePtr.
    T* release() __attribute__((warn_unused_result)) {
        T* result = mPtr;
        mPtr = NULL;
        return result;
    }

    // Takes ownership of the given raw pointer.
    // If this smart pointer previously owned a different raw pointer, that
    // raw pointer will be freed.
    void reset(T* ptr = NULL) {
        if (ptr != mPtr) {
            D()(mPtr);
            mPtr = ptr;
        }
    }

private:
    // The raw pointer.
    T* mPtr;

    // Comparing unique pointers is probably a mistake, since they're unique.
    template <typename T2> bool operator==(const UniquePtr<T2>& p) const;
    template <typename T2> bool operator!=(const UniquePtr<T2>& p) const;

    // Disallow copy and assignment.
    UniquePtr(const UniquePtr&);
    void operator=(const UniquePtr&);
};

// Partial specialization for array types. Like std::unique_ptr, this removes
// operator* and operator-> but adds operator[].
template <typename T, typename D>
class UniquePtr<T[], D> {
public:
    explicit UniquePtr(T* ptr = NULL) : mPtr(ptr) {
    }

    ~UniquePtr() {
        reset();
    }

    T& operator[](size_t i) const {
        return mPtr[i];
    }
    T* get() const { return mPtr; }

    T* release() __attribute__((warn_unused_result)) {
        T* result = mPtr;
        mPtr = NULL;
        return result;
    }

    void reset(T* ptr = NULL) {
        if (ptr != mPtr) {
            D()(mPtr);
            mPtr = ptr;
        }
    }

private:
    T* mPtr;

    // Disallow copy and assignment.
    UniquePtr(const UniquePtr&);
    void operator=(const UniquePtr&);
};

struct addrinfo_deleter {
    void operator()(addrinfo* p) const {
        if (p != NULL) { // bionic's freeaddrinfo(3) crashes when passed NULL.
            freeaddrinfo(p);
        }
    }
};

template <typename ScopedT>
class IoVec : public non_copyable_t {
public:
    IoVec(JNIEnv* env, size_t bufferCount) : mEnv(env), mBufferCount(bufferCount) {
        TRACE;
        mIoVec = new iovec[mBufferCount];
        mScopedBuffers = new ScopedT*[mBufferCount];
    }

    bool init(jobjectArray javaBuffers, jintArray javaOffsets, jintArray javaByteCounts) {
        TRACE;
        // We can't delete our local references until after the I/O, so make sure we have room.
        if (mEnv->PushLocalFrame(mBufferCount + 16) < 0) {
            return false;
        }
        ScopedIntArrayRO offsets(mEnv, javaOffsets);
        if (offsets.get() == NULL) {
            return false;
        }
        ScopedIntArrayRO byteCounts(mEnv, javaByteCounts);
        if (byteCounts.get() == NULL) {
            return false;
        }
        // TODO: Linux actually has a 1024 buffer limit. glibc works around this, and we should too.
        // TODO: you can query the limit at runtime with sysconf(_SC_IOV_MAX).
        for (size_t i = 0; i < mBufferCount; ++i) {
            jobject buffer = mEnv->GetObjectArrayElement(javaBuffers, i); // We keep this local ref.
            mScopedBuffers[i] = new ScopedT(mEnv, buffer);
            jbyte* ptr = const_cast<jbyte*>(mScopedBuffers[i]->get());
            if (ptr == NULL) {
                return false;
            }
            struct iovec iov;
            iov.iov_base = reinterpret_cast<void*>(ptr + offsets[i]);
            iov.iov_len = byteCounts[i];
            mIoVec[i] = iov;
        }
        return true;
    }

    ~IoVec() {
        TRACE;
        for (size_t i = 0; i < mBufferCount; ++i) {
            delete mScopedBuffers[i];
        }
        delete [] mScopedBuffers;
        delete [] mIoVec;
        mEnv->PopLocalFrame(NULL);
    }

    iovec* get() {
        return &mIoVec[0];
    }

    size_t size() {
        return mBufferCount;
    }

private:
    JNIEnv* mEnv;
    size_t mBufferCount;
    iovec *mIoVec;
    ScopedT** mScopedBuffers;
};

static jobject sockaddrToInetAddress(JNIEnv* env, const sockaddr_storage* ss, jint* port) {
    TRACE;
    // Convert IPv4-mapped IPv6 addresses to IPv4 addresses.
    // The RI states "Java will never return an IPv4-mapped address".
    sockaddr_storage tmp;
    memset(&tmp, 0, sizeof(tmp));
    const sockaddr_in6* sin6 = reinterpret_cast<const sockaddr_in6*>(ss);
    if (ss->ss_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED(&sin6->sin6_addr)) {
        // Copy the IPv6 address into the temporary sockaddr_storage.
        memcpy(&tmp, ss, sizeof(tmp));
        // Unmap it into an IPv4 address.
        sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(&tmp);
        sin->sin_family = AF_INET;
        sin->sin_port = sin6->sin6_port;
        memcpy(&sin->sin_addr.s_addr, &sin6->sin6_addr.s6_addr[12], 4);
        // Fall through into the regular conversion using the unmapped address.
        ss = &tmp;
    }

    const void* rawAddress;
    size_t addressLength;
    int sin_port;
    int scope_id = 0;
    if (ss->ss_family == AF_INET) {
        const sockaddr_in* sin = reinterpret_cast<const sockaddr_in*>(ss);
        rawAddress = &sin->sin_addr.s_addr;
        addressLength = 4;
        sin_port = ntohs(sin->sin_port);
    } else if (ss->ss_family == AF_INET6) {
        const sockaddr_in6* sin6 = reinterpret_cast<const sockaddr_in6*>(ss);
        rawAddress = &sin6->sin6_addr.s6_addr;
        addressLength = 16;
        sin_port = ntohs(sin6->sin6_port);
        scope_id = sin6->sin6_scope_id;
    } else {
        // We can't throw SocketException. We aren't meant to see bad addresses, so seeing one
        // really does imply an internal error.
        jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
                "sockaddrToInetAddress bad ss_family: %i", ss->ss_family);
        return NULL;
    }
    if (port != NULL) {
        *port = sin_port;
    }

    jni::jhbyteArray byteArray(env->NewByteArray(addressLength));
    if (byteArray.get() == NULL) {
        return NULL;
    }
    env->SetByteArrayRegion(byteArray.get(), 0, addressLength,
            reinterpret_cast<const jbyte*>(rawAddress));

    static jmethodID getByAddressMethod = env->GetStaticMethodID(inetAddressClass.get(),
            "getByAddress", "(Ljava/lang/String;[BI)Ljava/net/InetAddress;");
    if (getByAddressMethod == NULL) {
        return NULL;
    }
    return env->CallStaticObjectMethod(inetAddressClass.get(), getByAddressMethod,
            NULL, byteArray.get(), scope_id);
}

static bool inetAddressToSockaddr(JNIEnv* env, jobject inetAddress, int port, sockaddr_storage* ss, bool map) {
    TRACE;
    memset(ss, 0, sizeof(*ss));

    if (inetAddress == NULL) {
        jniThrowNullPointerException(env, NULL);
        return false;
    }

    // Get the address family.
    static jfieldID familyFid = env->GetFieldID(inetAddressClass.get(), "family", "I");
    ss->ss_family = env->GetIntField(inetAddress, familyFid);
    if (ss->ss_family == AF_UNSPEC) {
        return true; // Job done!
    }

    // Check this is an address family we support.
    if (ss->ss_family != AF_INET && ss->ss_family != AF_INET6) {
        jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
                "inetAddressToSockaddr bad family: %i", ss->ss_family);
        return false;
    }

    // Get the byte array that stores the IP address bytes in the InetAddress.
    static jfieldID bytesFid = env->GetFieldID(inetAddressClass.get(), "ipaddress", "[B");
    jni::jhbyteArray addressBytes(reinterpret_cast<jbyteArray>(env->GetObjectField(inetAddress, bytesFid)));
    if (addressBytes.get() == NULL) {
        jniThrowNullPointerException(env, NULL);
        return false;
    }

    // We use AF_INET6 sockets, so we want an IPv6 address (which may be a IPv4-mapped address).
    sockaddr_in6* sin6 = reinterpret_cast<sockaddr_in6*>(ss);
    sin6->sin6_port = htons(port);
    if (ss->ss_family == AF_INET6) {
        // IPv6 address. Copy the bytes...
        jbyte* dst = reinterpret_cast<jbyte*>(&sin6->sin6_addr.s6_addr);
        env->GetByteArrayRegion(addressBytes.get(), 0, 16, dst);
        // ...and set the scope id...
        static jfieldID scopeFid = env->GetFieldID(inet6AddressClass.get(), "scope_id", "I");
        sin6->sin6_scope_id = env->GetIntField(inetAddress, scopeFid);
        return true;
    }

    // Deal with Inet4Address instances.
    if (map) {
        // We should represent this Inet4Address as an IPv4-mapped IPv6 sockaddr_in6.
        // Change the family...
        sin6->sin6_family = AF_INET6;
        // Copy the bytes...
        jbyte* dst = reinterpret_cast<jbyte*>(&sin6->sin6_addr.s6_addr[12]);
        env->GetByteArrayRegion(addressBytes.get(), 0, 4, dst);
        // INADDR_ANY and in6addr_any are both all-zeros...
        if (!IN6_IS_ADDR_UNSPECIFIED(&sin6->sin6_addr)) {
            // ...but all other IPv4-mapped addresses are ::ffff:a.b.c.d, so insert the ffff...
            memset(&(sin6->sin6_addr.s6_addr[10]), 0xff, 2);
        }
    } else {
        // We should represent this Inet4Address as an IPv4 sockaddr_in.
        sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(ss);
        sin->sin_port = htons(port);
        jbyte* dst = reinterpret_cast<jbyte*>(&sin->sin_addr.s_addr);
        env->GetByteArrayRegion(addressBytes.get(), 0, 4, dst);
    }
    return true;
}

static bool inetAddressToSockaddrVerbatim(JNIEnv* env, jobject inetAddress, int port, sockaddr_storage* ss) {
    return inetAddressToSockaddr(env, inetAddress, port, ss, false);
}

static bool inetAddressToSockaddr(JNIEnv* env, jobject inetAddress, int port, sockaddr_storage* ss) {
    return inetAddressToSockaddr(env, inetAddress, port, ss, true);
}

static bool fillIfreq(JNIEnv* env, jstring javaInterfaceName, struct ifreq& req) {
    TRACE;
    ScopedUtfChars interfaceName(env, javaInterfaceName);
    if (interfaceName.c_str() == NULL) {
        return false;
    }
    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, interfaceName.c_str(), sizeof(req.ifr_name));
    req.ifr_name[sizeof(req.ifr_name) - 1] = '\0';
    return true;
}

static bool fillInetSocketAddress(JNIEnv* env, jint rc, jobject javaInetSocketAddress, const sockaddr_storage* ss) {
    TRACE;
    if (rc == -1 || javaInetSocketAddress == NULL) {
        return true;
    }
    // Fill out the passed-in InetSocketAddress with the sender's IP address and port number.
    jint port;
    jobject sender = sockaddrToInetAddress(env, ss, &port);
    if (sender == NULL) {
        return false;
    }
    static jfieldID addressFid = env->GetFieldID(inetSocketAddressClass.get(), "addr", "Ljava/net/InetAddress;");
    static jfieldID portFid = env->GetFieldID(inetSocketAddressClass.get(), "port", "I");
    env->SetObjectField(javaInetSocketAddress, addressFid, sender);
    env->SetIntField(javaInetSocketAddress, portFid, port);
    return true;
}

static jobject makeSocketAddress(JNIEnv* env, const sockaddr_storage* ss) {
    TRACE;
    // TODO: support AF_UNIX and AF_UNSPEC (and other families?)
    jint port;
    jobject inetAddress = sockaddrToInetAddress(env, ss, &port);
    if (inetAddress == NULL) {
        return NULL;
    }
    static jmethodID ctor = env->GetMethodID(inetSocketAddressClass.get(), "<init>",
            "(Ljava/net/InetAddress;I)V");
    return env->NewObject(inetSocketAddressClass.get(), ctor, inetAddress, port);
}

static jobject makeStructPasswd(JNIEnv* env, const struct passwd& pw) {
    TRACE;
    TO_JAVA_STRING(pw_name, pw.pw_name);
    TO_JAVA_STRING(pw_dir, pw.pw_dir);
    TO_JAVA_STRING(pw_shell, pw.pw_shell);
    static jmethodID ctor = env->GetMethodID(structPasswdClass.get(), "<init>",
            "(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
    return env->NewObject(structPasswdClass.get(), ctor,
            pw_name, static_cast<jint>(pw.pw_uid), static_cast<jint>(pw.pw_gid), pw_dir, pw_shell);
}

class Passwd {
public:
    Passwd(JNIEnv* env) : mEnv(env), mResult(NULL) {
        mBufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (mBufferSize == -1UL) {
            // We're probably on bionic, where 1KiB should be enough for anyone.
            // TODO: fix bionic to return 1024 like glibc.
            mBufferSize = 1024;
        }
        mBuffer.reset(new char[mBufferSize]);
    }

    jobject getpwnam(const char* name) {
        return process("getpwnam_r", system_getpwnam_r(name, &mPwd, mBuffer.get(), mBufferSize, &mResult));
    }

    jobject getpwuid(uid_t uid) {
        return process("getpwuid_r", system_getpwuid_r(uid, &mPwd, mBuffer.get(), mBufferSize, &mResult));
    }

    struct passwd* get() {
        return mResult;
    }

private:
    jobject process(const char* syscall, int error) {
        if (mResult == NULL) {
            errno = error;
            throwErrnoException(mEnv, syscall);
            return NULL;
        }
        return makeStructPasswd(mEnv, *mResult);
    }

    JNIEnv* mEnv;
    UniquePtr<char[]> mBuffer;
    size_t mBufferSize;
    struct passwd mPwd;
    struct passwd* mResult;
};

static jobject makeStructStat(JNIEnv* env, const struct stat& sb) {
    TRACE;
    static jmethodID ctor = env->GetMethodID(structStatClass.get(), "<init>",
            "(JJIJIIJJJJJJJ)V");
    return env->NewObject(structStatClass.get(), ctor,
            static_cast<jlong>(sb.st_dev), static_cast<jlong>(sb.st_ino),
            static_cast<jint>(sb.st_mode), static_cast<jlong>(sb.st_nlink),
            static_cast<jint>(sb.st_uid), static_cast<jint>(sb.st_gid),
            static_cast<jlong>(sb.st_rdev), static_cast<jlong>(sb.st_size),
            static_cast<jlong>(sb.st_atime), static_cast<jlong>(sb.st_mtime),
            static_cast<jlong>(sb.st_ctime), static_cast<jlong>(sb.st_blksize),
            static_cast<jlong>(sb.st_blocks));
}

static jobject doStat(JNIEnv* env, jstring javaPath, bool isLstat) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return NULL;
    }
    struct stat sb;
    int rc = isLstat ? TEMP_FAILURE_RETRY(::lstat(path.c_str(), &sb))
                     : TEMP_FAILURE_RETRY(::stat(path.c_str(), &sb));
    if (rc == -1) {
        throwErrnoException(env, isLstat ? "lstat" : "stat");
        return NULL;
    }
    return makeStructStat(env, sb);
}

static jobject makeStructStatFs(JNIEnv* env, const struct statfs& sb) {
    TRACE;
    static_assert(sizeof(sb.f_bavail) == sizeof(jlong), "statfs_not_64_bit");
    static_assert(sizeof(sb.f_bfree) == sizeof(jlong), "statfs_not_64_bit");
    static_assert(sizeof(sb.f_blocks) == sizeof(jlong), "statfs_not_64_bit");

    static jmethodID ctor = env->GetMethodID(structStatFsClass.get(), "<init>",
            "(JJJJJJJJ)V");
    return env->NewObject(structStatFsClass.get(), ctor, static_cast<jlong>(sb.f_bsize),
            static_cast<jlong>(sb.f_blocks), static_cast<jlong>(sb.f_bfree),
            static_cast<jlong>(sb.f_bavail), static_cast<jlong>(sb.f_files),
            static_cast<jlong>(sb.f_ffree), static_cast<jlong>(sb.f_namelen),
            static_cast<jlong>(sb.f_frsize));
}

static jobject makeStructLinger(JNIEnv* env, const struct linger& l) {
    TRACE;
    static jmethodID ctor = env->GetMethodID(structLingerClass.get(), "<init>", "(II)V");
    return env->NewObject(structLingerClass.get(), ctor, l.l_onoff, l.l_linger);
}

static jobject makeStructTimeval(JNIEnv* env, const struct timeval& tv) {
    TRACE;
    static jmethodID ctor = env->GetMethodID(structTimevalClass.get(), "<init>", "(JJ)V");
    return env->NewObject(structTimevalClass.get(), ctor,
            static_cast<jlong>(tv.tv_sec), static_cast<jlong>(tv.tv_usec));
}

static jobject makeStructUtsname(JNIEnv* env, const struct utsname& buf) {
    TRACE;
    TO_JAVA_STRING(sysname, buf.sysname);
    TO_JAVA_STRING(nodename, buf.nodename);
    TO_JAVA_STRING(release, buf.release);
    TO_JAVA_STRING(version, buf.version);
    TO_JAVA_STRING(machine, buf.machine);
    static jmethodID ctor = env->GetMethodID(structUtsnameClass.get(), "<init>",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    return env->NewObject(structUtsnameClass.get(), ctor,
            sysname, nodename, release, version, machine);
}

static jboolean Posix_access(JNIEnv* env, jobject, jstring javaPath, jint mode) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return JNI_FALSE;
    }
    int rc = TEMP_FAILURE_RETRY(::access(path.c_str(), mode));
    if (rc == -1) {
        throwErrnoException(env, "access");
    }
    return (rc == 0);
}

static jobject Posix_accept(JNIEnv* env, jobject, jobject javaFd, jobject javaInetSocketAddress) {
    TRACE;
    sockaddr_storage ss;
    socklen_t sl = sizeof(ss);
    memset(&ss, 0, sizeof(ss));
    int fd;
    sockaddr* peer = (javaInetSocketAddress != NULL) ? reinterpret_cast<sockaddr*>(&ss) : NULL;
    socklen_t* peerLength = (javaInetSocketAddress != NULL) ? &sl : 0;
    jint clientFd = NET_FAILURE_RETRY("accept", accept(fd, peer, peerLength));
    if (clientFd == -1 || !fillInetSocketAddress(env, clientFd, javaInetSocketAddress, &ss)) {
        close(clientFd);
        return NULL;
    }
    return (clientFd != -1) ? jniCreateFileDescriptor(env, clientFd) : NULL;
}

static void Posix_bind(JNIEnv* env, jobject, jobject javaFd, jobject javaAddress, jint port) {
    TRACE;
    sockaddr_storage ss;
    if (!inetAddressToSockaddr(env, javaAddress, port, &ss)) {
        return;
    }
    int fd;
    const sockaddr* sa = reinterpret_cast<const sockaddr*>(&ss);
    NET_FAILURE_RETRY("bind", bind(fd, sa, sizeof(sockaddr_storage)));
}

static void Posix_chmod(JNIEnv* env, jobject, jstring javaPath, jint mode) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return;
    }
    throwIfMinusOne(env, "chmod", TEMP_FAILURE_RETRY(::chmod(path.c_str(), mode)));
}

static void Posix_close(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    // Get the FileDescriptor's 'fd' field and clear it.
    // We need to do this before we can throw an IOException (http://b/3222087).
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    jniSetFileDescriptorOfFD(env, javaFd, -1);

    // Even if close(2) fails with EINTR, the fd will have been closed.
    // Using TEMP_FAILURE_RETRY will either lead to EBADF or closing someone else's fd.
    // http://lkml.indiana.edu/hypermail/linux/kernel/0509.1/0877.html
    throwIfMinusOne(env, "close", ::close(fd));
}

static void Posix_connect(JNIEnv* env, jobject, jobject javaFd, jobject javaAddress, jint port) {
    TRACE;
    sockaddr_storage ss;
    if (!inetAddressToSockaddr(env, javaAddress, port, &ss)) {
        return;
    }
    int fd;
    const sockaddr* sa = reinterpret_cast<const sockaddr*>(&ss);
    NET_FAILURE_RETRY("connect", connect(fd, sa, sizeof(sockaddr_storage)));
}

static jobject Posix_dup(JNIEnv* env, jobject, jobject javaOldFd) {
    TRACE;
    int oldFd = jniGetFDFromFileDescriptor(env, javaOldFd);
    DBG("oldFd=%d", oldFd);
    int newFd = throwIfMinusOne(env, "dup", TEMP_FAILURE_RETRY(::dup(oldFd)));
    DBG("newFd=%d", newFd);
    return (newFd != -1) ? jniCreateFileDescriptor(env, newFd) : NULL;
}

static jobject Posix_dup2(JNIEnv* env, jobject, jobject javaOldFd, jint newFd) {
    TRACE;
    int oldFd = jniGetFDFromFileDescriptor(env, javaOldFd);
    DBG("oldFd=%d", oldFd);
    int fd = throwIfMinusOne(env, "dup2", TEMP_FAILURE_RETRY(::dup2(oldFd, newFd)));
    DBG("fd=%d", fd);
    return (fd != -1) ? jniCreateFileDescriptor(env, fd) : NULL;
}

static jobjectArray Posix_environ(JNIEnv* env, jobject) {
    return toStringArray(env, environ);
}

static jint Posix_fcntlVoid(JNIEnv* env, jobject, jobject javaFd, jint cmd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "fcntl", TEMP_FAILURE_RETRY(::fcntl(fd, cmd)));
}

static jint Posix_fcntlLong(JNIEnv* env, jobject, jobject javaFd, jint cmd, jlong arg) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "fcntl", TEMP_FAILURE_RETRY(::fcntl(fd, cmd, arg)));
}

static jint Posix_fcntlFlock(JNIEnv* env, jobject, jobject javaFd, jint cmd, jobject javaFlock) {
    TRACE;
    static jfieldID typeFid = env->GetFieldID(structFlockClass.get(), "l_type", "S");
    static jfieldID whenceFid = env->GetFieldID(structFlockClass.get(), "l_whence", "S");
    static jfieldID startFid = env->GetFieldID(structFlockClass.get(), "l_start", "J");
    static jfieldID lenFid = env->GetFieldID(structFlockClass.get(), "l_len", "J");
    static jfieldID pidFid = env->GetFieldID(structFlockClass.get(), "l_pid", "I");

    struct flock64 lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = env->GetShortField(javaFlock, typeFid);
    lock.l_whence = env->GetShortField(javaFlock, whenceFid);
    lock.l_start = env->GetLongField(javaFlock, startFid);
    lock.l_len = env->GetLongField(javaFlock, lenFid);
    lock.l_pid = env->GetIntField(javaFlock, pidFid);

    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    int rc = throwIfMinusOne(env, "fcntl", TEMP_FAILURE_RETRY(::fcntl(fd, cmd, &lock)));
    if (rc != -1) {
        env->SetShortField(javaFlock, typeFid, lock.l_type);
        env->SetShortField(javaFlock, whenceFid, lock.l_whence);
        env->SetLongField(javaFlock, startFid, lock.l_start);
        env->SetLongField(javaFlock, lenFid, lock.l_len);
        env->SetIntField(javaFlock, pidFid, lock.l_pid);
    }
    return rc;
}

static void Posix_fdatasync(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "fdatasync", TEMP_FAILURE_RETRY(::fdatasync(fd)));
}

static jobject Posix_fstat(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct stat sb;
    int rc = TEMP_FAILURE_RETRY(::fstat(fd, &sb));
    if (rc == -1) {
        throwErrnoException(env, "fstat");
        return NULL;
    }
    return makeStructStat(env, sb);
}

static jobject Posix_fstatfs(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct statfs sb;
    int rc = TEMP_FAILURE_RETRY(::fstatfs(fd, &sb));
    if (rc == -1) {
        throwErrnoException(env, "fstatfs");
        return NULL;
    }
    return makeStructStatFs(env, sb);
}

static void Posix_fsync(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "fsync", TEMP_FAILURE_RETRY(::fsync(fd)));
}

static void Posix_ftruncate(JNIEnv* env, jobject, jobject javaFd, jlong length) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "ftruncate", TEMP_FAILURE_RETRY(::ftruncate(fd, length)));
}

static jstring Posix_gai_strerror(JNIEnv* env, jobject, jint error) {
    TRACE;
    return env->NewStringUTF(gai_strerror(error));
}

static jobjectArray Posix_getaddrinfo(JNIEnv* env, jobject, jstring javaNode, jobject javaHints) {
    ScopedUtfChars node(env, javaNode);
    if (node.c_str() == NULL) {
        return NULL;
    }

    static jfieldID flagsFid = env->GetFieldID(structAddrinfoClass.get(), "ai_flags", "I");
    static jfieldID familyFid = env->GetFieldID(structAddrinfoClass.get(), "ai_family", "I");
    static jfieldID socktypeFid = env->GetFieldID(structAddrinfoClass.get(), "ai_socktype", "I");
    static jfieldID protocolFid = env->GetFieldID(structAddrinfoClass.get(), "ai_protocol", "I");

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = env->GetIntField(javaHints, flagsFid);
    hints.ai_family = env->GetIntField(javaHints, familyFid);
    hints.ai_socktype = env->GetIntField(javaHints, socktypeFid);
    hints.ai_protocol = env->GetIntField(javaHints, protocolFid);

    addrinfo* addressList = NULL;
    int rc = getaddrinfo(node.c_str(), NULL, &hints, &addressList);
    UniquePtr<addrinfo, addrinfo_deleter> addressListDeleter(addressList);
    if (rc != 0) {
        throwGaiException(env, "getaddrinfo", rc);
        return NULL;
    }

    // Count results so we know how to size the output array.
    int addressCount = 0;
    for (addrinfo* ai = addressList; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_family == AF_INET || ai->ai_family == AF_INET6) {
            ++addressCount;
        } else {
            ERR("getaddrinfo unexpected ai_family %i", ai->ai_family);
        }
    }
    if (addressCount == 0) {
        return NULL;
    }

    // Prepare output array.
    jobjectArray result = env->NewObjectArray(addressCount, inetAddressClass.get(), NULL);
    if (result == NULL) {
        return NULL;
    }

    // Examine returned addresses one by one, save them in the output array.
    int index = 0;
    for (addrinfo* ai = addressList; ai != NULL; ai = ai->ai_next) {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6) {
            // Unknown address family. Skip this address.
            ERR("getaddrinfo unexpected ai_family %i", ai->ai_family);
            continue;
        }

        // Convert each IP address into a Java byte array.
        sockaddr_storage* address = reinterpret_cast<sockaddr_storage*>(ai->ai_addr);
        jni::jhobject inetAddress(sockaddrToInetAddress(env, address, NULL));
        if (inetAddress.get() == NULL) {
            return NULL;
        }
        env->SetObjectArrayElement(result, index, inetAddress.get());
        ++index;
    }
    return result;
}

static jint Posix_getegid(JNIEnv*, jobject) {
    TRACE;
    return getegid();
}

static jint Posix_geteuid(JNIEnv*, jobject) {
    TRACE;
    return geteuid();
}

static jint Posix_getgid(JNIEnv*, jobject) {
    TRACE;
    return getgid();
}

static jstring Posix_getenv(JNIEnv* env, jobject, jstring javaName) {
    TRACE;
    ScopedUtfChars name(env, javaName);
    if (name.c_str() == NULL) {
        return NULL;
    }
    return env->NewStringUTF(getenv(name.c_str()));
}

static jstring Posix_getnameinfo(JNIEnv* env, jobject, jobject javaAddress, jint flags) {
    TRACE;
    sockaddr_storage ss;
    if (!inetAddressToSockaddrVerbatim(env, javaAddress, 0, &ss)) {
        return NULL;
    }
    // TODO: bionic's getnameinfo(3) seems to want its length parameter to be exactly
    // sizeof(sockaddr_in) for an IPv4 address and sizeof (sockaddr_in6) for an
    // IPv6 address. Fix getnameinfo so it accepts sizeof(sockaddr_storage), and
    // then remove this hack.
    socklen_t size = (ss.ss_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
    char buf[NI_MAXHOST]; // NI_MAXHOST is longer than INET6_ADDRSTRLEN.
    int rc = getnameinfo(reinterpret_cast<sockaddr*>(&ss), size, buf, sizeof(buf), NULL, 0, flags);
    if (rc != 0) {
        throwGaiException(env, "getnameinfo", rc);
        return NULL;
    }
    return env->NewStringUTF(buf);
}

static jint Posix_getpid(JNIEnv*, jobject) {
    TRACE;
    return getpid();
}

static jint Posix_getppid(JNIEnv*, jobject) {
    TRACE;
    return getppid();
}

static jobject Posix_getpwnam(JNIEnv* env, jobject, jstring javaName) {
    TRACE;
    ScopedUtfChars name(env, javaName);
    if (name.c_str() == NULL) {
        return NULL;
    }
    return Passwd(env).getpwnam(name.c_str());
}

static jobject Posix_getpwuid(JNIEnv* env, jobject, jint uid) {
    TRACE;
    return Passwd(env).getpwuid(uid);
}

static jobject Posix_getsockname(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    sockaddr_storage ss;
    sockaddr* sa = reinterpret_cast<sockaddr*>(&ss);
    socklen_t byteCount = sizeof(ss);
    memset(&ss, 0, byteCount);
    int rc = TEMP_FAILURE_RETRY(getsockname(fd, sa, &byteCount));
    if (rc == -1) {
        throwErrnoException(env, "getsockname");
        return NULL;
    }
    return makeSocketAddress(env, &ss);
}

static jint Posix_getsockoptByte(JNIEnv* env, jobject, jobject javaFd, jint level, jint option) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    u_char result = 0;
    socklen_t size = sizeof(result);
    throwIfMinusOne(env, "getsockopt", TEMP_FAILURE_RETRY(getsockopt(fd, level, option, &result, &size)));
    return result;
}

static jobject Posix_getsockoptInAddr(JNIEnv* env, jobject, jobject javaFd, jint level, jint option) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    sockaddr_storage ss;
    memset(&ss, 0, sizeof(ss));
    ss.ss_family = AF_INET; // This is only for the IPv4-only IP_MULTICAST_IF.
    sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(&ss);
    socklen_t size = sizeof(sa->sin_addr);
    int rc = TEMP_FAILURE_RETRY(getsockopt(fd, level, option, &sa->sin_addr, &size));
    if (rc == -1) {
        throwErrnoException(env, "getsockopt");
        return NULL;
    }
    return sockaddrToInetAddress(env, &ss, NULL);
}

static jint Posix_getsockoptInt(JNIEnv* env, jobject, jobject javaFd, jint level, jint option) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    jint result = 0;
    socklen_t size = sizeof(result);
    throwIfMinusOne(env, "getsockopt", TEMP_FAILURE_RETRY(getsockopt(fd, level, option, &result, &size)));
    return result;
}

static jobject Posix_getsockoptLinger(JNIEnv* env, jobject, jobject javaFd, jint level, jint option) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct linger l;
    socklen_t size = sizeof(l);
    memset(&l, 0, size);
    int rc = TEMP_FAILURE_RETRY(getsockopt(fd, level, option, &l, &size));
    if (rc == -1) {
        throwErrnoException(env, "getsockopt");
        return NULL;
    }
    return makeStructLinger(env, l);
}

static jobject Posix_getsockoptTimeval(JNIEnv* env, jobject, jobject javaFd, jint level, jint option) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct timeval tv;
    socklen_t size = sizeof(tv);
    memset(&tv, 0, size);
    int rc = TEMP_FAILURE_RETRY(getsockopt(fd, level, option, &tv, &size));
    if (rc == -1) {
        throwErrnoException(env, "getsockopt");
        return NULL;
    }
    return makeStructTimeval(env, tv);
}

static jint Posix_getuid(JNIEnv*, jobject) {
    TRACE;
    return getuid();
}

static jstring Posix_if_indextoname(JNIEnv* env, jobject, jint index) {
    TRACE;
    char buf[IF_NAMESIZE];
    char* name = if_indextoname(index, buf);
    // if_indextoname(3) returns NULL on failure, which will come out of NewStringUTF unscathed.
    // There's no useful information in errno, so we don't bother throwing. Callers can null-check.
    return env->NewStringUTF(name);
}

static jobject Posix_inet_pton(JNIEnv* env, jobject, jint family, jstring javaName) {
    TRACE;
    ScopedUtfChars name(env, javaName);
    if (name.c_str() == NULL) {
        return NULL;
    }
    sockaddr_storage ss;
    memset(&ss, 0, sizeof(ss));
    // sockaddr_in and sockaddr_in6 are at the same address, so we can use either here.
    void* dst = &reinterpret_cast<sockaddr_in*>(&ss)->sin_addr;
    if (inet_pton(family, name.c_str(), dst) != 1) {
        return NULL;
    }
    ss.ss_family = family;
    return sockaddrToInetAddress(env, &ss, NULL);
}

static jobject Posix_ioctlInetAddress(JNIEnv* env, jobject, jobject javaFd, jint cmd, jstring javaInterfaceName) {
    TRACE;
    struct ifreq req;
    if (!fillIfreq(env, javaInterfaceName, req)) {
        return NULL;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    int rc = throwIfMinusOne(env, "ioctl", TEMP_FAILURE_RETRY(ioctl(fd, cmd, &req)));
    if (rc == -1) {
        return NULL;
    }
    return sockaddrToInetAddress(env, reinterpret_cast<sockaddr_storage*>(&req.ifr_addr), NULL);
}

static jint Posix_ioctlInt(JNIEnv* env, jobject, jobject javaFd, jint cmd, jobject javaArg) {
    TRACE;
    // This is complicated because ioctls may return their result by updating their argument
    // or via their return value, so we need to support both.
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    static jfieldID valueFid = env->GetFieldID(mutableIntClass.get(), "value", "I");
    jint arg = env->GetIntField(javaArg, valueFid);
    int rc = throwIfMinusOne(env, "ioctl", TEMP_FAILURE_RETRY(::ioctl(fd, cmd, &arg)));
    if (!env->ExceptionCheck()) {
        env->SetIntField(javaArg, valueFid, arg);
    }
    return rc;
}

static jboolean Posix_isatty(JNIEnv* env, jobject, jobject javaFd) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return TEMP_FAILURE_RETRY(isatty(fd)) == 0;
}

static void Posix_kill(JNIEnv* env, jobject, jint pid, jint sig) {
    TRACE;
    throwIfMinusOne(env, "kill", TEMP_FAILURE_RETRY(kill(pid, sig)));
}

static void Posix_listen(JNIEnv* env, jobject, jobject javaFd, jint backlog) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "listen", TEMP_FAILURE_RETRY(listen(fd, backlog)));
}

static jlong Posix_lseek(JNIEnv* env, jobject, jobject javaFd, jlong offset, jint whence) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "lseek", TEMP_FAILURE_RETRY(::lseek64(fd, offset, whence)));
}

static jobject Posix_lstat(JNIEnv* env, jobject, jstring javaPath) {
    TRACE;
    return doStat(env, javaPath, true);
}

static void Posix_mincore(JNIEnv* env, jobject, jlong address, jlong byteCount, jbyteArray javaVector) {
    TRACE;
    ScopedByteArrayRW vector(env, javaVector);
    if (vector.get() == NULL) {
        return;
    }
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    unsigned char* vec = reinterpret_cast<unsigned char*>(vector.get());
    throwIfMinusOne(env, "mincore", TEMP_FAILURE_RETRY(mincore(ptr, byteCount, vec)));
}

static void Posix_mkdir(JNIEnv* env, jobject, jstring javaPath, jint mode) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return;
    }
    throwIfMinusOne(env, "mkdir", TEMP_FAILURE_RETRY(::mkdir(path.c_str(), mode)));
}

static void Posix_mlock(JNIEnv* env, jobject, jlong address, jlong byteCount) {
    TRACE;
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    throwIfMinusOne(env, "mlock", TEMP_FAILURE_RETRY(mlock(ptr, byteCount)));
}

static jlong Posix_mmap(JNIEnv* env, jobject, jlong address, jlong byteCount, jint prot, jint flags, jobject javaFd, jlong offset) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    void* suggestedPtr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    void* ptr = mmap(suggestedPtr, byteCount, prot, flags, fd, offset);
    if (ptr == MAP_FAILED) {
        throwErrnoException(env, "mmap");
    }
    return static_cast<jlong>(reinterpret_cast<uintptr_t>(ptr));
}

static void Posix_msync(JNIEnv* env, jobject, jlong address, jlong byteCount, jint flags) {
    TRACE;
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    throwIfMinusOne(env, "msync", TEMP_FAILURE_RETRY(msync(ptr, byteCount, flags)));
}

static void Posix_munlock(JNIEnv* env, jobject, jlong address, jlong byteCount) {
    TRACE;
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    throwIfMinusOne(env, "munlock", TEMP_FAILURE_RETRY(munlock(ptr, byteCount)));
}

static void Posix_munmap(JNIEnv* env, jobject, jlong address, jlong byteCount) {
    TRACE;
    void* ptr = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
    throwIfMinusOne(env, "munmap", TEMP_FAILURE_RETRY(munmap(ptr, byteCount)));
}

static jobject Posix_open(JNIEnv* env, jobject, jstring javaPath, jint flags, jint mode) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return NULL;
    }
    int fd = throwIfMinusOne(env, "open", TEMP_FAILURE_RETRY(::open(path.c_str(), flags, mode)));
    DBG("fd=%d", fd);
    return fd != -1 ? jniCreateFileDescriptor(env, fd) : NULL;
}

static jobjectArray Posix_pipe(JNIEnv* env, jobject) {
    TRACE;
    int fds[2];
    throwIfMinusOne(env, "pipe", TEMP_FAILURE_RETRY(pipe(&fds[0])));
    jobjectArray result = env->NewObjectArray(2, clsFD.get(), NULL);
    if (result == NULL) {
        return NULL;
    }
    for (int i = 0; i < 2; ++i) {
        jni::jhobject fd(jniCreateFileDescriptor(env, fds[i]));
        if (fd.get() == NULL) {
            return NULL;
        }
        env->SetObjectArrayElement(result, i, fd.get());
        if (env->ExceptionCheck()) {
            return NULL;
        }
    }
    return result;
}

static jint Posix_poll(JNIEnv* env, jobject, jobjectArray javaStructs, jint timeoutMs) {
    TRACE;
    static jfieldID fdFid = env->GetFieldID(structPollfdClass.get(), "fd", "Ljava/io/FileDescriptor;");
    static jfieldID eventsFid = env->GetFieldID(structPollfdClass.get(), "events", "S");
    static jfieldID reventsFid = env->GetFieldID(structPollfdClass.get(), "revents", "S");

    // Turn the Java libcore.io.StructPollfd[] into a C++ struct pollfd[].
    size_t arrayLength = env->GetArrayLength(javaStructs);
    UniquePtr<struct pollfd[]> fds(new struct pollfd[arrayLength]);
    memset(fds.get(), 0, sizeof(struct pollfd) * arrayLength);
    size_t count = 0; // Some trailing array elements may be irrelevant. (See below.)
    for (size_t i = 0; i < arrayLength; ++i) {
        jni::jhobject javaStruct(env->GetObjectArrayElement(javaStructs, i));
        if (javaStruct.get() == NULL) {
            break; // We allow trailing nulls in the array for caller convenience.
        }
        jni::jhobject javaFd(env->GetObjectField(javaStruct.get(), fdFid));
        if (javaFd.get() == NULL) {
            break; // We also allow callers to just clear the fd field (this is what Selector does).
        }
        fds[count].fd = jniGetFDFromFileDescriptor(env, javaFd.get());
        fds[count].events = env->GetShortField(javaStruct.get(), eventsFid);
        ++count;
    }

    int rc = TEMP_FAILURE_RETRY(poll(fds.get(), count, timeoutMs));
    if (rc == -1) {
        throwErrnoException(env, "poll");
        return -1;
    }

    // Update the revents fields in the Java libcore.io.StructPollfd[].
    for (size_t i = 0; i < count; ++i) {
        jni::jhobject javaStruct(env->GetObjectArrayElement(javaStructs, i));
        if (javaStruct.get() == NULL) {
            return -1;
        }
        env->SetShortField(javaStruct.get(), reventsFid, fds[i].revents);
    }
    return rc;
}

static jint Posix_preadBytes(JNIEnv* env, jobject, jobject javaFd, jobject javaBytes, jint byteOffset, jint byteCount, jlong offset) {
    TRACE;
    ScopedBytesRW bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "pread", TEMP_FAILURE_RETRY(::pread(fd, bytes.get() + byteOffset, byteCount, offset)));
}

static jint Posix_pwriteBytes(JNIEnv* env, jobject, jobject javaFd, jbyteArray javaBytes, jint byteOffset, jint byteCount, jlong offset) {
    TRACE;
    ScopedBytesRO bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "pwrite", TEMP_FAILURE_RETRY(::pwrite(fd, bytes.get() + byteOffset, byteCount, offset)));
}

static jint Posix_readBytes(JNIEnv* env, jobject, jobject javaFd, jobject javaBytes, jint byteOffset, jint byteCount) {
    TRACE;
    ScopedBytesRW bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "read", TEMP_FAILURE_RETRY(::read(fd, bytes.get() + byteOffset, byteCount)));
}

static jint Posix_readv(JNIEnv* env, jobject, jobject javaFd, jobjectArray buffers, jintArray offsets, jintArray byteCounts) {
    TRACE;
    IoVec<ScopedBytesRW> ioVec(env, env->GetArrayLength(buffers));
    if (!ioVec.init(buffers, offsets, byteCounts)) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "readv", TEMP_FAILURE_RETRY(::readv(fd, ioVec.get(), ioVec.size())));
}

static jint Posix_recvfromBytes(JNIEnv* env, jobject, jobject javaFd, jobject javaBytes, jint byteOffset, jint byteCount, jint flags, jobject javaInetSocketAddress) {
    TRACE;
    ScopedBytesRW bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    sockaddr_storage ss;
    socklen_t sl = sizeof(ss);
    memset(&ss, 0, sizeof(ss));
    int fd;
    sockaddr* from = (javaInetSocketAddress != NULL) ? reinterpret_cast<sockaddr*>(&ss) : NULL;
    socklen_t* fromLength = (javaInetSocketAddress != NULL) ? &sl : 0;
    jint recvCount = NET_FAILURE_RETRY("recvfrom", recvfrom(fd, bytes.get() + byteOffset, byteCount, flags, from, fromLength));
    fillInetSocketAddress(env, recvCount, javaInetSocketAddress, &ss);
    return recvCount;
}

static void Posix_remove(JNIEnv* env, jobject, jstring javaPath) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return;
    }
    throwIfMinusOne(env, "remove", TEMP_FAILURE_RETRY(::remove(path.c_str())));
}

static void Posix_rename(JNIEnv* env, jobject, jstring javaOldPath, jstring javaNewPath) {
    TRACE;
    ScopedUtfChars oldPath(env, javaOldPath);
    if (oldPath.c_str() == NULL) {
        return;
    }
    ScopedUtfChars newPath(env, javaNewPath);
    if (newPath.c_str() == NULL) {
        return;
    }
    throwIfMinusOne(env, "rename", TEMP_FAILURE_RETRY(::rename(oldPath.c_str(), newPath.c_str())));
}

static jlong Posix_sendfile(JNIEnv* env, jobject, jobject javaOutFd, jobject javaInFd, jobject javaOffset, jlong byteCount) {
    TRACE;
    int outFd = jniGetFDFromFileDescriptor(env, javaOutFd);
    DBG("outFd=%d", outFd);
    int inFd = jniGetFDFromFileDescriptor(env, javaInFd);
    DBG("inFd=%d", inFd);
    static jfieldID valueFid = env->GetFieldID(mutableLongClass.get(), "value", "J");
    off_t offset = 0;
    off_t* offsetPtr = NULL;
    if (javaOffset != NULL) {
        // TODO: fix bionic so we can have a 64-bit off_t!
        offset = env->GetLongField(javaOffset, valueFid);
        offsetPtr = &offset;
    }
    jlong result = throwIfMinusOne(env, "sendfile", TEMP_FAILURE_RETRY(sendfile(outFd, inFd, offsetPtr, byteCount)));
    if (javaOffset != NULL) {
        env->SetLongField(javaOffset, valueFid, offset);
    }
    return result;
}

static jint Posix_sendtoBytes(JNIEnv* env, jobject, jobject javaFd, jobject javaBytes, jint byteOffset, jint byteCount, jint flags, jobject javaInetAddress, jint port) {
    TRACE;
    ScopedBytesRO bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    sockaddr_storage ss;
    if (javaInetAddress != NULL && !inetAddressToSockaddr(env, javaInetAddress, port, &ss)) {
        return -1;
    }
    int fd;
    const sockaddr* to = (javaInetAddress != NULL) ? reinterpret_cast<const sockaddr*>(&ss) : NULL;
    socklen_t toLength = (javaInetAddress != NULL) ? sizeof(ss) : 0;
    return NET_FAILURE_RETRY("sendto", sendto(fd, bytes.get() + byteOffset, byteCount, flags, to, toLength));
}

static void Posix_setegid(JNIEnv* env, jobject, jint egid) {
    TRACE;
    throwIfMinusOne(env, "setegid", TEMP_FAILURE_RETRY(setegid(egid)));
}

static void Posix_seteuid(JNIEnv* env, jobject, jint euid) {
    TRACE;
    throwIfMinusOne(env, "seteuid", TEMP_FAILURE_RETRY(seteuid(euid)));
}

static void Posix_setgid(JNIEnv* env, jobject, jint gid) {
    TRACE;
    throwIfMinusOne(env, "setgid", TEMP_FAILURE_RETRY(setgid(gid)));
}

static void Posix_setsockoptByte(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jint value) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    u_char byte = value;
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &byte, sizeof(byte))));
}

static void Posix_setsockoptIfreq(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jstring javaInterfaceName) {
    TRACE;
    struct ifreq req;
    if (!fillIfreq(env, javaInterfaceName, req)) {
        return;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &req, sizeof(req))));
}

static void Posix_setsockoptInt(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jint value) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &value, sizeof(value))));
}

static void Posix_setsockoptIpMreqn(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jint value) {
    TRACE;
    ip_mreqn req;
    memset(&req, 0, sizeof(req));
    req.imr_ifindex = value;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &req, sizeof(req))));
}

static void Posix_setsockoptGroupReq(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jobject javaGroupReq) {
    TRACE;
    struct group_req req;
    memset(&req, 0, sizeof(req));

    static jfieldID grInterfaceFid = env->GetFieldID(structGroupReqClass.get(), "gr_interface", "I");
    req.gr_interface = env->GetIntField(javaGroupReq, grInterfaceFid);
    // Get the IPv4 or IPv6 multicast address to join or leave.
    static jfieldID grGroupFid = env->GetFieldID(structGroupReqClass.get(), "gr_group", "Ljava/net/InetAddress;");
    jni::jhobject javaGroup(env->GetObjectField(javaGroupReq, grGroupFid));
    if (!inetAddressToSockaddrVerbatim(env, javaGroup.get(), 0, &req.gr_group)) {
        return;
    }

    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    int rc = TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &req, sizeof(req)));
    if (rc == -1 && errno == EINVAL) {
        // Maybe we're a 32-bit binary talking to a 64-bit kernel?
        // glibc doesn't automatically handle this.
        struct group_req64 {
            uint32_t gr_interface;
            uint32_t my_padding;
            sockaddr_storage gr_group;
        };
        group_req64 req64;
        req64.gr_interface = req.gr_interface;
        memcpy(&req64.gr_group, &req.gr_group, sizeof(req.gr_group));
        rc = TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &req64, sizeof(req64)));
    }
    throwIfMinusOne(env, "setsockopt", rc);
}

static void Posix_setsockoptLinger(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jobject javaLinger) {
    TRACE;
    static jfieldID lOnoffFid = env->GetFieldID(structLingerClass.get(), "l_onoff", "I");
    static jfieldID lLingerFid = env->GetFieldID(structLingerClass.get(), "l_linger", "I");
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct linger value;
    value.l_onoff = env->GetIntField(javaLinger, lOnoffFid);
    value.l_linger = env->GetIntField(javaLinger, lLingerFid);
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &value, sizeof(value))));
}

static void Posix_setsockoptTimeval(JNIEnv* env, jobject, jobject javaFd, jint level, jint option, jobject javaTimeval) {
    TRACE;
    static jfieldID tvSecFid = env->GetFieldID(structTimevalClass.get(), "tv_sec", "J");
    static jfieldID tvUsecFid = env->GetFieldID(structTimevalClass.get(), "tv_usec", "J");
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    struct timeval value;
    value.tv_sec = env->GetLongField(javaTimeval, tvSecFid);
    value.tv_usec = env->GetLongField(javaTimeval, tvUsecFid);
    throwIfMinusOne(env, "setsockopt", TEMP_FAILURE_RETRY(setsockopt(fd, level, option, &value, sizeof(value))));
}

static void Posix_setuid(JNIEnv* env, jobject, jint uid) {
    TRACE;
    throwIfMinusOne(env, "setuid", TEMP_FAILURE_RETRY(setuid(uid)));
}

static void Posix_shutdown(JNIEnv* env, jobject, jobject javaFd, jint how) {
    TRACE;
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    throwIfMinusOne(env, "shutdown", TEMP_FAILURE_RETRY(shutdown(fd, how)));
}

static jobject Posix_socket(JNIEnv* env, jobject, jint domain, jint type, jint protocol) {
    TRACE;
    int fd = throwIfMinusOne(env, "socket", TEMP_FAILURE_RETRY(socket(domain, type, protocol)));
    DBG("fd=%d", fd);
    return fd != -1 ? jniCreateFileDescriptor(env, fd) : NULL;
}

static jobject Posix_stat(JNIEnv* env, jobject, jstring javaPath) {
    TRACE;
    return doStat(env, javaPath, false);
}

static jobject Posix_statfs(JNIEnv* env, jobject, jstring javaPath) {
    TRACE;
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return NULL;
    }
    struct statfs sb;
    int rc = TEMP_FAILURE_RETRY(statfs(path.c_str(), &sb));
    if (rc == -1) {
        throwErrnoException(env, "statfs");
        return NULL;
    }
    return makeStructStatFs(env, sb);
}

static jstring Posix_strerror(JNIEnv* env, jobject, jint errnum) {
    TRACE;
    char buffer[BUFSIZ];
    const char* message = jniStrError(errnum, buffer, sizeof(buffer));
    return env->NewStringUTF(message);
}

static void Posix_symlink(JNIEnv* env, jobject, jstring javaOldPath, jstring javaNewPath) {
    TRACE;
    ScopedUtfChars oldPath(env, javaOldPath);
    if (oldPath.c_str() == NULL) {
        return;
    }
    ScopedUtfChars newPath(env, javaNewPath);
    if (newPath.c_str() == NULL) {
        return;
    }
    throwIfMinusOne(env, "symlink", TEMP_FAILURE_RETRY(::symlink(oldPath.c_str(), newPath.c_str())));
}

static jlong Posix_sysconf(JNIEnv* env, jobject, jint name) {
    TRACE;
    // Since -1 is a valid result from sysconf(3), detecting failure is a little more awkward.
    errno = 0;
    long result = sysconf(name);
    if (result == -1L && errno == EINVAL) {
        throwErrnoException(env, "sysconf");
    }
    return result;
}

static jobject Posix_uname(JNIEnv* env, jobject) {
    TRACE;
    struct utsname buf;
    if (TEMP_FAILURE_RETRY(uname(&buf)) == -1) {
        return NULL; // Can't happen.
    }
    return makeStructUtsname(env, buf);
}

static jint Posix_waitpid(JNIEnv* env, jobject, jint pid, jobject javaStatus, jint options) {
    TRACE;
    int status;
    int rc = throwIfMinusOne(env, "waitpid", TEMP_FAILURE_RETRY(waitpid(pid, &status, options)));
    if (rc != -1) {
        static jfieldID valueFid = env->GetFieldID(mutableIntClass.get(), "value", "I");
        env->SetIntField(javaStatus, valueFid, status);
    }
    return rc;
}

static jint Posix_writeBytes(JNIEnv* env, jobject, jobject javaFd, jbyteArray javaBytes, jint byteOffset, jint byteCount) {
    TRACE;
    ScopedBytesRO bytes(env, javaBytes);
    if (bytes.get() == NULL) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "write", TEMP_FAILURE_RETRY(::write(fd, bytes.get() + byteOffset, byteCount)));
}

static jint Posix_writev(JNIEnv* env, jobject, jobject javaFd, jobjectArray buffers, jintArray offsets, jintArray byteCounts) {
    TRACE;
    IoVec<ScopedBytesRO> ioVec(env, env->GetArrayLength(buffers));
    if (!ioVec.init(buffers, offsets, byteCounts)) {
        return -1;
    }
    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    DBG("fd=%d", fd);
    return throwIfMinusOne(env, "writev", TEMP_FAILURE_RETRY(::writev(fd, ioVec.get(), ioVec.size())));
}

static JNINativeMethod posix_methods[] = {
    NATIVE_METHOD(Posix, accept, "(Ljava/io/FileDescriptor;Ljava/net/InetSocketAddress;)Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, access, "(Ljava/lang/String;I)Z"),
    NATIVE_METHOD(Posix, bind, "(Ljava/io/FileDescriptor;Ljava/net/InetAddress;I)V"),
    NATIVE_METHOD(Posix, chmod, "(Ljava/lang/String;I)V"),
    NATIVE_METHOD(Posix, close, "(Ljava/io/FileDescriptor;)V"),
    NATIVE_METHOD(Posix, connect, "(Ljava/io/FileDescriptor;Ljava/net/InetAddress;I)V"),
    NATIVE_METHOD(Posix, dup, "(Ljava/io/FileDescriptor;)Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, dup2, "(Ljava/io/FileDescriptor;I)Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, environ, "()[Ljava/lang/String;"),
    NATIVE_METHOD(Posix, fcntlVoid, "(Ljava/io/FileDescriptor;I)I"),
    NATIVE_METHOD(Posix, fcntlLong, "(Ljava/io/FileDescriptor;IJ)I"),
    NATIVE_METHOD(Posix, fcntlFlock, "(Ljava/io/FileDescriptor;ILlibcore/io/StructFlock;)I"),
    NATIVE_METHOD(Posix, fdatasync, "(Ljava/io/FileDescriptor;)V"),
    NATIVE_METHOD(Posix, fstat, "(Ljava/io/FileDescriptor;)Llibcore/io/StructStat;"),
    NATIVE_METHOD(Posix, fstatfs, "(Ljava/io/FileDescriptor;)Llibcore/io/StructStatFs;"),
    NATIVE_METHOD(Posix, fsync, "(Ljava/io/FileDescriptor;)V"),
    NATIVE_METHOD(Posix, ftruncate, "(Ljava/io/FileDescriptor;J)V"),
    NATIVE_METHOD(Posix, gai_strerror, "(I)Ljava/lang/String;"),
    NATIVE_METHOD(Posix, getaddrinfo, "(Ljava/lang/String;Llibcore/io/StructAddrinfo;)[Ljava/net/InetAddress;"),
    NATIVE_METHOD(Posix, getegid, "()I"),
    NATIVE_METHOD(Posix, geteuid, "()I"),
    NATIVE_METHOD(Posix, getgid, "()I"),
    NATIVE_METHOD(Posix, getenv, "(Ljava/lang/String;)Ljava/lang/String;"),
    NATIVE_METHOD(Posix, getnameinfo, "(Ljava/net/InetAddress;I)Ljava/lang/String;"),
    NATIVE_METHOD(Posix, getpid, "()I"),
    NATIVE_METHOD(Posix, getppid, "()I"),
    NATIVE_METHOD(Posix, getpwnam, "(Ljava/lang/String;)Llibcore/io/StructPasswd;"),
    NATIVE_METHOD(Posix, getpwuid, "(I)Llibcore/io/StructPasswd;"),
    NATIVE_METHOD(Posix, getsockname, "(Ljava/io/FileDescriptor;)Ljava/net/SocketAddress;"),
    NATIVE_METHOD(Posix, getsockoptByte, "(Ljava/io/FileDescriptor;II)I"),
    NATIVE_METHOD(Posix, getsockoptInAddr, "(Ljava/io/FileDescriptor;II)Ljava/net/InetAddress;"),
    NATIVE_METHOD(Posix, getsockoptInt, "(Ljava/io/FileDescriptor;II)I"),
    NATIVE_METHOD(Posix, getsockoptLinger, "(Ljava/io/FileDescriptor;II)Llibcore/io/StructLinger;"),
    NATIVE_METHOD(Posix, getsockoptTimeval, "(Ljava/io/FileDescriptor;II)Llibcore/io/StructTimeval;"),
    NATIVE_METHOD(Posix, getuid, "()I"),
    NATIVE_METHOD(Posix, if_indextoname, "(I)Ljava/lang/String;"),
    NATIVE_METHOD(Posix, inet_pton, "(ILjava/lang/String;)Ljava/net/InetAddress;"),
    NATIVE_METHOD(Posix, ioctlInetAddress, "(Ljava/io/FileDescriptor;ILjava/lang/String;)Ljava/net/InetAddress;"),
    NATIVE_METHOD(Posix, ioctlInt, "(Ljava/io/FileDescriptor;ILlibcore/util/MutableInt;)I"),
    NATIVE_METHOD(Posix, isatty, "(Ljava/io/FileDescriptor;)Z"),
    NATIVE_METHOD(Posix, kill, "(II)V"),
    NATIVE_METHOD(Posix, listen, "(Ljava/io/FileDescriptor;I)V"),
    NATIVE_METHOD(Posix, lseek, "(Ljava/io/FileDescriptor;JI)J"),
    NATIVE_METHOD(Posix, lstat, "(Ljava/lang/String;)Llibcore/io/StructStat;"),
    NATIVE_METHOD(Posix, mincore, "(JJ[B)V"),
    NATIVE_METHOD(Posix, mkdir, "(Ljava/lang/String;I)V"),
    NATIVE_METHOD(Posix, mlock, "(JJ)V"),
    NATIVE_METHOD(Posix, mmap, "(JJIILjava/io/FileDescriptor;J)J"),
    NATIVE_METHOD(Posix, msync, "(JJI)V"),
    NATIVE_METHOD(Posix, munlock, "(JJ)V"),
    NATIVE_METHOD(Posix, munmap, "(JJ)V"),
    NATIVE_METHOD(Posix, open, "(Ljava/lang/String;II)Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, pipe, "()[Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, poll, "([Llibcore/io/StructPollfd;I)I"),
    NATIVE_METHOD(Posix, preadBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;IIJ)I"),
    NATIVE_METHOD(Posix, pwriteBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;IIJ)I"),
    NATIVE_METHOD(Posix, readBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;II)I"),
    NATIVE_METHOD(Posix, readv, "(Ljava/io/FileDescriptor;[Ljava/lang/Object;[I[I)I"),
    NATIVE_METHOD(Posix, recvfromBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;IIILjava/net/InetSocketAddress;)I"),
    NATIVE_METHOD(Posix, remove, "(Ljava/lang/String;)V"),
    NATIVE_METHOD(Posix, rename, "(Ljava/lang/String;Ljava/lang/String;)V"),
    NATIVE_METHOD(Posix, sendfile, "(Ljava/io/FileDescriptor;Ljava/io/FileDescriptor;Llibcore/util/MutableLong;J)J"),
    NATIVE_METHOD(Posix, sendtoBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;IIILjava/net/InetAddress;I)I"),
    NATIVE_METHOD(Posix, setegid, "(I)V"),
    NATIVE_METHOD(Posix, seteuid, "(I)V"),
    NATIVE_METHOD(Posix, setgid, "(I)V"),
    NATIVE_METHOD(Posix, setsockoptByte, "(Ljava/io/FileDescriptor;III)V"),
    NATIVE_METHOD(Posix, setsockoptIfreq, "(Ljava/io/FileDescriptor;IILjava/lang/String;)V"),
    NATIVE_METHOD(Posix, setsockoptInt, "(Ljava/io/FileDescriptor;III)V"),
    NATIVE_METHOD(Posix, setsockoptIpMreqn, "(Ljava/io/FileDescriptor;III)V"),
    NATIVE_METHOD(Posix, setsockoptGroupReq, "(Ljava/io/FileDescriptor;IILlibcore/io/StructGroupReq;)V"),
    NATIVE_METHOD(Posix, setsockoptLinger, "(Ljava/io/FileDescriptor;IILlibcore/io/StructLinger;)V"),
    NATIVE_METHOD(Posix, setsockoptTimeval, "(Ljava/io/FileDescriptor;IILlibcore/io/StructTimeval;)V"),
    NATIVE_METHOD(Posix, setuid, "(I)V"),
    NATIVE_METHOD(Posix, shutdown, "(Ljava/io/FileDescriptor;I)V"),
    NATIVE_METHOD(Posix, socket, "(III)Ljava/io/FileDescriptor;"),
    NATIVE_METHOD(Posix, stat, "(Ljava/lang/String;)Llibcore/io/StructStat;"),
    NATIVE_METHOD(Posix, statfs, "(Ljava/lang/String;)Llibcore/io/StructStatFs;"),
    NATIVE_METHOD(Posix, strerror, "(I)Ljava/lang/String;"),
    NATIVE_METHOD(Posix, symlink, "(Ljava/lang/String;Ljava/lang/String;)V"),
    NATIVE_METHOD(Posix, sysconf, "(I)J"),
    NATIVE_METHOD(Posix, uname, "()Llibcore/io/StructUtsname;"),
    NATIVE_METHOD(Posix, waitpid, "(ILlibcore/util/MutableInt;I)I"),
    NATIVE_METHOD(Posix, writeBytes, "(Ljava/io/FileDescriptor;Ljava/lang/Object;II)I"),
    NATIVE_METHOD(Posix, writev, "(Ljava/io/FileDescriptor;[Ljava/lang/Object;[I[I)I"),
};

CRYSTAX_LOCAL
bool init(JNIEnv *env)
{
    DBG("init ICS classes");
    byteArrayClass = jni::find_class(env, "[B");
    errnoExceptionClass = jni::find_class(env, "libcore/io/ErrnoException");
    gaiExceptionClass = jni::find_class(env, "libcore/io/GaiException");
    inet6AddressClass = jni::find_class(env, "java/net/Inet6Address");
    inetAddressClass = jni::find_class(env, "java/net/InetAddress");
    inetSocketAddressClass = jni::find_class(env, "java/net/InetSocketAddress");
    mutableIntClass = jni::find_class(env, "libcore/util/MutableInt");
    mutableLongClass = jni::find_class(env, "libcore/util/MutableLong");
    stringClass = jni::find_class(env, "java/lang/String");
    structAddrinfoClass = jni::find_class(env, "libcore/io/StructAddrinfo");
    structFlockClass = jni::find_class(env, "libcore/io/StructFlock");
    structGroupReqClass = jni::find_class(env, "libcore/io/StructGroupReq");
    structLingerClass = jni::find_class(env, "libcore/io/StructLinger");
    structPasswdClass = jni::find_class(env, "libcore/io/StructPasswd");
    structPollfdClass = jni::find_class(env, "libcore/io/StructPollfd");
    structStatClass = jni::find_class(env, "libcore/io/StructStat");
    structStatFsClass = jni::find_class(env, "libcore/io/StructStatFs");
    structTimevalClass = jni::find_class(env, "libcore/io/StructTimeval");
    structUtsnameClass = jni::find_class(env, "libcore/io/StructUtsname");

    DBG("trying to register Posix (ics implementation) native methods...");
    const char *clsname = "libcore/io/Posix";
    if (!register_native_methods(env, clsname,
            posix_methods, sizeof(posix_methods)/sizeof(posix_methods[0])))
    {
        ERR("can't register Posix native methods");
        return false;
    }
    DBG("Posix (gingerbread implementation) native methods registered");

    return true;
}

} // namespace ics
} // namespace osfs
} // namespace fileio
} // namespace crystax
