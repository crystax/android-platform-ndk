/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
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

namespace crystax
{
namespace fileio
{
namespace osfs
{
namespace gingerbread
{

static jint OSFileSystem_lockImpl(JNIEnv* env, jobject, jint fd,
        jlong start, jlong length, jint typeFlag, jboolean waitFlag) {

    DBG("fd=%d, start=%lld, length=%lld", (int)fd, (long long)start, (long long)length);
    length = translateLockLength(length);
    if (offsetTooLarge(env, start) || offsetTooLarge(env, length)) {
        return -1;
    }

    struct flock lock(flockFromStartAndLength(start, length));

    if ((typeFlag & SHARED_LOCK_TYPE) == SHARED_LOCK_TYPE) {
        lock.l_type = F_RDLCK;
    } else {
        lock.l_type = F_WRLCK;
    }

    int waitMode = (waitFlag) ? F_SETLKW : F_SETLK;
    return TEMP_FAILURE_RETRY(::fcntl(fd, waitMode, &lock));
}

static void OSFileSystem_unlockImpl(JNIEnv* env, jobject, jint fd, jlong start, jlong length) {
    DBG("fd=%d, start=%lld, length=%lld", (int)fd, (long long)start, (long long)length);
    length = translateLockLength(length);
    if (offsetTooLarge(env, start) || offsetTooLarge(env, length)) {
        return;
    }

    struct flock lock(flockFromStartAndLength(start, length));
    lock.l_type = F_UNLCK;

    int rc = TEMP_FAILURE_RETRY(::fcntl(fd, F_SETLKW, &lock));
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
}

/**
 * Returns the granularity of the starting address for virtual memory allocation.
 * (It's the same as the page size.)
 */
static jint OSFileSystem_getAllocGranularity(JNIEnv*, jobject) {
    TRACE;
    static int allocGranularity = getpagesize();
    return allocGranularity;
}

// Translate three Java int[]s to a native iovec[] for readv and writev.
static iovec* initIoVec(JNIEnv* env,
        jintArray jBuffers, jintArray jOffsets, jintArray jLengths, jint size) {
    TRACE;
    scope_cpp_array_t<iovec> vectors(new iovec[size]);
    if (vectors.get() == NULL) {
        jniThrowException(env, "java/lang/OutOfMemoryError", "native heap");
        return NULL;
    }
    ScopedIntArrayRO buffers(env, jBuffers);
    if (buffers.get() == NULL) {
        return NULL;
    }
    ScopedIntArrayRO offsets(env, jOffsets);
    if (offsets.get() == NULL) {
        return NULL;
    }
    ScopedIntArrayRO lengths(env, jLengths);
    if (lengths.get() == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; ++i) {
        vectors[i].iov_base = reinterpret_cast<void*>(buffers[i] + offsets[i]);
        vectors[i].iov_len = lengths[i];
    }
    return vectors.release();
}

static jlong OSFileSystem_readv(JNIEnv* env, jobject, jint fd,
        jintArray jBuffers, jintArray jOffsets, jintArray jLengths, jint size) {
    DBG("fd=%d", (int)fd);
    scope_cpp_array_t<iovec> vectors(initIoVec(env, jBuffers, jOffsets, jLengths, size));
    if (vectors.get() == NULL) {
        return -1;
    }
    long result = ::readv(fd, vectors.get(), size);
    if (result == 0) {
        return -1;
    }
    if (result == -1) {
        jniThrowIOException(env, errno);
    }
    return result;
}

static jlong OSFileSystem_writev(JNIEnv* env, jobject, jint fd,
        jintArray jBuffers, jintArray jOffsets, jintArray jLengths, jint size) {
    DBG("fd=%d", (int)fd);
    scope_cpp_array_t<iovec> vectors(initIoVec(env, jBuffers, jOffsets, jLengths, size));
    if (vectors.get() == NULL) {
        return -1;
    }
    long result = ::writev(fd, vectors.get(), size);
    if (result == -1) {
        jniThrowIOException(env, errno);
    }
    return result;
}

static jlong OSFileSystem_transfer(JNIEnv* env, jobject, jint fd, jobject sd,
        jlong offset, jlong count) {

    DBG("fd=%d", (int)fd);
    int socket = jniGetFDFromFileDescriptor(env, sd);
    if (socket == -1) {
        return -1;
    }

    /* Value of offset is checked in jint scope (checked in java layer)
       The conversion here is to guarantee no value lost when converting offset to off_t
     */
    off_t off = offset;

    ssize_t rc = ::sendfile(socket, fd, &off, count);
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
    return rc;
}

static jlong OSFileSystem_readDirect(JNIEnv* env, jobject, jint fd,
        jint buf, jint offset, jint byteCount) {
    DBG("fd=%d", (int)fd);
    if (byteCount == 0) {
        return 0;
    }
    jbyte* dst = reinterpret_cast<jbyte*>(buf + offset);
    jlong rc = TEMP_FAILURE_RETRY(::read(fd, dst, byteCount));
    if (rc == 0) {
        return -1;
    }
    if (rc == -1) {
        // We return 0 rather than throw if we try to read from an empty non-blocking pipe.
        if (errno == EAGAIN) {
            return 0;
        }
        jniThrowIOException(env, errno);
    }
    return rc;
}

static jlong OSFileSystem_read(JNIEnv* env, jobject, jint fd,
        jbyteArray byteArray, jint offset, jint byteCount) {
    DBG("fd=%d", (int)fd);
    ScopedByteArrayRW bytes(env, byteArray);
    if (bytes.get() == NULL) {
        return 0;
    }
    jint buf = static_cast<jint>(reinterpret_cast<uintptr_t>(bytes.get()));
    return OSFileSystem_readDirect(env, NULL, fd, buf, offset, byteCount);
}

static jlong OSFileSystem_writeDirect(JNIEnv* env, jobject, jint fd,
        jint buf, jint offset, jint byteCount) {
    DBG("fd=%d", (int)fd);
    if (byteCount == 0) {
        return 0;
    }
    jbyte* src = reinterpret_cast<jbyte*>(buf + offset);
    jlong rc = TEMP_FAILURE_RETRY(::write(fd, src, byteCount));
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
    return rc;
}

static jlong OSFileSystem_write(JNIEnv* env, jobject, jint fd,
        jbyteArray byteArray, jint offset, jint byteCount) {
    DBG("fd=%d", (int)fd);
    ScopedByteArrayRO bytes(env, byteArray);
    if (bytes.get() == NULL) {
        return 0;
    }
    jint buf = static_cast<jint>(reinterpret_cast<uintptr_t>(bytes.get()));
    return OSFileSystem_writeDirect(env, NULL, fd, buf, offset, byteCount);
}

static jlong OSFileSystem_seek(JNIEnv* env, jobject, jint fd, jlong offset, jint javaWhence) {
    DBG("fd=%d", (int)fd);
    /* Convert whence argument */
    int nativeWhence = 0;
    switch (javaWhence) {
    case 1:
        nativeWhence = SEEK_SET;
        break;
    case 2:
        nativeWhence = SEEK_CUR;
        break;
    case 4:
        nativeWhence = SEEK_END;
        break;
    default:
        return -1;
    }

    // If the offset is relative, lseek(2) will tell us whether it's too large.
    // We're just worried about too large an absolute offset, which would cause
    // us to lie to lseek(2).
    if (offsetTooLarge(env, offset)) {
        return -1;
    }

    jlong result = ::lseek(fd, offset, nativeWhence);
    if (result == -1) {
        jniThrowIOException(env, errno);
    }
    return result;
}

static void OSFileSystem_fsync(JNIEnv* env, jobject, jint fd, jboolean metadataToo) {
    DBG("fd=%d", (int)fd);
    int rc = metadataToo ? ::fsync(fd) : ::fdatasync(fd);
    // int rc = metadataToo ? fsync(fd) : fdatasync(fd);
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
}

static jint OSFileSystem_truncate(JNIEnv* env, jobject, jint fd, jlong length) {
    DBG("fd=%d", (int)fd);
    if (offsetTooLarge(env, length)) {
        return -1;
    }

    int rc = ::ftruncate(fd, length);
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
    return rc;
}

static jint OSFileSystem_open(JNIEnv* env, jobject, jstring javaPath, jint jflags) {
    TRACE;
    int flags = 0;
    int mode = 0;

    // On Android, we don't want default permissions to allow global access.
    switch (jflags) {
    case 0:
        flags = HyOpenRead;
        mode = 0;
        break;
    case 1:
        flags = HyOpenCreate | HyOpenWrite | HyOpenTruncate;
        mode = 0600;
        break;
    case 16:
        flags = HyOpenRead | HyOpenWrite | HyOpenCreate;
        mode = 0600;
        break;
    case 32:
        flags = HyOpenRead | HyOpenWrite | HyOpenCreate | HyOpenSync;
        mode = 0600;
        break;
    case 256:
        flags = HyOpenWrite | HyOpenCreate | HyOpenAppend;
        mode = 0600;
        break;
    }

    flags = EsTranslateOpenFlags(flags);

    path_t path(jni::jcast<const char *>(javaPath));
    if (path.c_str() == NULL) {
        return -1;
    }
    DBG("path=%s", path.c_str());
    jint rc = TEMP_FAILURE_RETRY(::open(path.c_str(), flags, mode));
    if (rc == -1) {
        // Get the human-readable form of errno.
        char buffer[80];
        const char* reason = jniStrError(errno, &buffer[0], sizeof(buffer));

        // Construct a message that includes the path and the reason.
        size_t len = path.length() + 2 + ::strlen(reason) + 1 + 1;
        scope_cpp_array_t<char> message(new char[len]);
        snprintf(&message[0], len, "%s (%s)", path.c_str(), reason);

        // We always throw FileNotFoundException, regardless of the specific
        // failure. (This appears to be true of the RI too.)
        jniThrowException(env, "java/io/FileNotFoundException", &message[0]);
    }
    DBG("path=%s: return fd=%d", path.c_str(), rc);
    return rc;
}

static jint OSFileSystem_ioctlAvailable(JNIEnv*env, jobject, jobject fileDescriptor) {
    TRACE;
    /*
     * On underlying platforms Android cares about (read "Linux"),
     * ioctl(fd, FIONREAD, &avail) is supposed to do the following:
     *
     * If the fd refers to a regular file, avail is set to
     * the difference between the file size and the current cursor.
     * This may be negative if the cursor is past the end of the file.
     *
     * If the fd refers to an open socket or the read end of a
     * pipe, then avail will be set to a number of bytes that are
     * available to be read without blocking.
     *
     * If the fd refers to a special file/device that has some concept
     * of buffering, then avail will be set in a corresponding way.
     *
     * If the fd refers to a special device that does not have any
     * concept of buffering, then the ioctl call will return a negative
     * number, and errno will be set to ENOTTY.
     *
     * If the fd refers to a special file masquerading as a regular file,
     * then avail may be returned as negative, in that the special file
     * may appear to have zero size and yet a previous read call may have
     * actually read some amount of data and caused the cursor to be
     * advanced.
     */
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    DBG("fd=%d", fd);
    if (fd == -1) {
        return -1;
    }
    int avail = 0;
    int rc = ::ioctl(fd, FIONREAD, &avail);
    if (rc >= 0) {
        /*
         * Success, but make sure not to return a negative number (see
         * above).
         */
        if (avail < 0) {
            avail = 0;
        }
    } else if (errno == ENOTTY) {
        /* The fd is unwilling to opine about its read buffer. */
        avail = 0;
    } else {
        /* Something strange is happening. */
        jniThrowIOException(env, errno);
    }

    return avail;
}

static jlong OSFileSystem_length(JNIEnv* env, jobject, jint fd) {
    DBG("fd=%d", (int)fd);
    struct stat sb;
    jint rc = TEMP_FAILURE_RETRY(::fstat(fd, &sb));
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
    return sb.st_size;
}

static void IoUtils_close(JNIEnv* env, jclass, jobject fileDescriptor) {
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    DBG("fd=%d", fd);
    jint rc = TEMP_FAILURE_RETRY(::close(fd));
    if (rc == -1) {
        jniThrowIOException(env, errno);
    }
    jniSetFileDescriptorOfFD(env, fileDescriptor, -1);
}

static jint IoUtils_getFd(JNIEnv* env, jclass, jobject fileDescriptor) {
    return jniGetFDFromFileDescriptor(env, fileDescriptor);
}

static void IoUtils_pipe(JNIEnv* env, jclass, jintArray javaFds) {
    ScopedIntArrayRW fds(env, javaFds);
    if (fds.get() == NULL) {
        return;
    }
    int rc = ::pipe(&fds[0]);
    if (rc == -1) {
        jniThrowIOException(env, errno);
        return;
    }
}

static void IoUtils_setFd(JNIEnv* env, jclass, jobject fileDescriptor, jint newValue) {
    DBG("newValue=%d", newValue);
    return jniSetFileDescriptorOfFD(env, fileDescriptor, newValue);
}

static bool setBlocking(int fd, bool blocking) {
    DBG("fd=%d", fd);
    int flags = ::fcntl(fd, F_GETFL);
    if (flags == -1) {
        return false;
    }

    if (!blocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    int rc = ::fcntl(fd, F_SETFL, flags);
    return (rc != -1);
}

static void IoUtils_setBlocking(JNIEnv* env, jclass, jobject fileDescriptor, jboolean blocking) {
    int fd = jniGetFDFromFileDescriptor(env, fileDescriptor);
    DBG("fd=%d", fd);
    if (fd == -1) {
        return;
    }
    if (!setBlocking(fd, blocking)) {
        jniThrowIOException(env, errno);
    }
}

static JNINativeMethod osfs_methods[] = {
    NATIVE_METHOD(OSFileSystem, fsync, "(IZ)V"),
    NATIVE_METHOD(OSFileSystem, getAllocGranularity, "()I"),
    NATIVE_METHOD(OSFileSystem, ioctlAvailable, "(Ljava/io/FileDescriptor;)I"),
    NATIVE_METHOD(OSFileSystem, length, "(I)J"),
    NATIVE_METHOD(OSFileSystem, lockImpl, "(IJJIZ)I"),
    NATIVE_METHOD(OSFileSystem, open, "(Ljava/lang/String;I)I"),
    NATIVE_METHOD(OSFileSystem, read, "(I[BII)J"),
    NATIVE_METHOD(OSFileSystem, readDirect, "(IIII)J"),
    NATIVE_METHOD(OSFileSystem, readv, "(I[I[I[II)J"),
    NATIVE_METHOD(OSFileSystem, seek, "(IJI)J"),
    NATIVE_METHOD(OSFileSystem, transfer, "(ILjava/io/FileDescriptor;JJ)J"),
    NATIVE_METHOD(OSFileSystem, truncate, "(IJ)V"),
    NATIVE_METHOD(OSFileSystem, unlockImpl, "(IJJ)V"),
    NATIVE_METHOD(OSFileSystem, write, "(I[BII)J"),
    NATIVE_METHOD(OSFileSystem, writeDirect, "(IIII)J"),
    NATIVE_METHOD(OSFileSystem, writev, "(I[I[I[II)J"),
};

static JNINativeMethod ioutils_methods[] = {
    NATIVE_METHOD(IoUtils, close, "(Ljava/io/FileDescriptor;)V"),
    NATIVE_METHOD(IoUtils, getFd, "(Ljava/io/FileDescriptor;)I"),
    NATIVE_METHOD(IoUtils, pipe, "([I)V"),
    NATIVE_METHOD(IoUtils, setFd, "(Ljava/io/FileDescriptor;I)V"),
    NATIVE_METHOD(IoUtils, setBlocking, "(Ljava/io/FileDescriptor;Z)V"),
};

CRYSTAX_LOCAL
bool init(JNIEnv *env)
{
    DBG("trying to register IoUtils (gingerbread implementation) native methods...");
    const char *ioutilsname = "libcore/io/IoUtils";
    if (!register_native_methods(env, ioutilsname,
            ioutils_methods, sizeof(ioutils_methods)/sizeof(ioutils_methods[0])))
    {
        ERR("can't register IoUtils native methods");
        return false;
    }
    DBG("IoUtils (gingerbread implementation) native methods registered");

    DBG("trying to register OSFileSystem (gingerbread implementation) native methods...");
    const char *osfsname = "org/apache/harmony/luni/platform/OSFileSystem";
    if (!register_native_methods(env, osfsname,
            osfs_methods, sizeof(osfs_methods)/sizeof(osfs_methods[0])))
    {
        ERR("can't register OSFileSystem native methods");
        return false;
    }
    DBG("OSFileSystem (gingerbread implementation) native methods registered");

    return true;
}

} // namespace gingerbread
} // namespace osfs
} // namespace fileio
} // namespace crystax
