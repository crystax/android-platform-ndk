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
namespace eclair
{

static void convertToPlatform(char *path) {
    char *pathIndex;

    pathIndex = path;
    while (*pathIndex != '\0') {
        if(*pathIndex == '\\') {
            *pathIndex = '/';
        }
        pathIndex++;
    }
}

static jint OSFileSystem_lockImpl(JNIEnv * env, jobject thiz, jint handle, 
        jlong start, jlong length, jint typeFlag, jboolean waitFlag) {

    DBG("fd=%d", handle);
    int rc;
    int waitMode = (waitFlag) ? F_SETLKW : F_SETLK;
    struct flock lock;

    ::memset(&lock, 0, sizeof(lock));

    // If start or length overflow the max values we can represent, then max them out.
    if(start > 0x7fffffffL) {
        start = 0x7fffffffL;
    }
    if(length > 0x7fffffffL) {
        length = 0x7fffffffL;
    }

    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = length;

    if((typeFlag & SHARED_LOCK_TYPE) == SHARED_LOCK_TYPE) {
        lock.l_type = F_RDLCK;
    } else {
        lock.l_type = F_WRLCK;
    }

    do {
        rc = ::fcntl(handle, waitMode, &lock);
    } while ((rc < 0) && (errno == EINTR));

    return (rc == -1) ? -1 : 0;
}

static jint OSFileSystem_getAllocGranularity(JNIEnv * env, jobject thiz) {
    TRACE;
    static int allocGranularity = 0;
    if(allocGranularity == 0) {
        allocGranularity = ::getpagesize();
    }
    return allocGranularity;
}

static jint OSFileSystem_unlockImpl(JNIEnv * env, jobject thiz, jint handle, 
        jlong start, jlong length) {

    DBG("fd=%d", handle);

    int rc;
    struct flock lock;

    ::memset(&lock, 0, sizeof(lock));

    // If start or length overflow the max values we can represent, then max them out.
    if(start > 0x7fffffffL) {
        start = 0x7fffffffL;
    }
    if(length > 0x7fffffffL) {
        length = 0x7fffffffL;
    }

    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = length;
    lock.l_type = F_UNLCK;

    do {
        rc = ::fcntl(handle, F_SETLKW, &lock);
    } while ((rc < 0) && (errno == EINTR));

    return (rc == -1) ? -1 : 0;
}

static jint OSFileSystem_fflushImpl(JNIEnv * env, jobject thiz, jint fd, 
        jboolean metadata) {
    DBG("fd=%d", fd);
    return (jint) ::fsync(fd);
}

static jlong OSFileSystem_seekImpl(JNIEnv * env, jobject thiz, jint fd, 
        jlong offset, jint whence) {

    DBG("fd=%d", fd);
    int mywhence = 0;

    /* Convert whence argument */
    switch (whence) {
        case 1:
                mywhence = 0;
                break;
        case 2:
                mywhence = 1;
                break;
        case 4:
                mywhence = 2;
                break;
        default:
                return -1;
    }


    off_t localOffset = (int) offset;

    if((mywhence < 0) || (mywhence > 2)) {
        return -1;
    }

    /* If file offsets are 32 bit, truncate the seek to that range */
    if(sizeof (off_t) < sizeof (jlong)) {
        if(offset > 0x7FFFFFFF) {
            localOffset = 0x7FFFFFFF;
        } else if(offset < -0x7FFFFFFF) {
            localOffset = -0x7FFFFFFF;
        }
    }

    return (jlong) ::lseek(fd, localOffset, mywhence);
}

static jlong OSFileSystem_readDirectImpl(JNIEnv * env, jobject thiz, jint fd, 
        jint buf, jint offset, jint nbytes) {
    DBG("fd=%d", fd);
    jint result;
    if(nbytes == 0) {
        return (jlong) 0;
    }

    result = ::read(fd, (void *) ((jint *)(buf+offset)), (int) nbytes);
    if(result == 0) {
        return (jlong) -1;
    } else {
        return (jlong) result;
    }
}

static jlong OSFileSystem_writeDirectImpl(JNIEnv * env, jobject thiz, jint fd, 
        jint buf, jint offset, jint nbytes) {

    DBG("fd=%d", fd);

    int rc = 0;

    /* write will just do the right thing for HYPORT_TTY_OUT and HYPORT_TTY_ERR */
    rc = ::write (fd, (const void *) ((jint *)(buf+offset)), (int) nbytes);

    if(rc == -1) {
        jniThrowException(env, "java/io/IOException", ::strerror(errno));
        return -2;
    }
    return (jlong) rc;

}

static jlong OSFileSystem_readImpl(JNIEnv * env, jobject thiz, jint fd, 
        jbyteArray byteArray, jint offset, jint nbytes) {

    DBG("fd=%d", fd);
    jboolean isCopy;
    jbyte *bytes;
    jlong result;

    if (nbytes == 0) {
        return 0;
    }

    bytes = env->GetByteArrayElements(byteArray, &isCopy);

    for (;;) {
        result = ::read(fd, (void *) (bytes + offset), (int) nbytes);

        if ((result != -1) || (errno != EINTR)) {
            break;
        }

        /*
         * If we didn't break above, that means that the read() call
         * returned due to EINTR. We shield Java code from this
         * possibility by trying again. Note that this is different
         * from EAGAIN, which should result in this code throwing
         * an InterruptedIOException.
         */
    }

    env->ReleaseByteArrayElements(byteArray, bytes, 0);

    if (result == 0) {
        return -1;
    }
    
    if (result == -1) {
        if (errno == EAGAIN) {
            jniThrowException(env, "java/io/InterruptedIOException",
                    "Read timed out");
        } else {
        jniThrowException(env, "java/io/IOException", ::strerror(errno));
        }
    }

    return result;
}

static jlong OSFileSystem_writeImpl(JNIEnv * env, jobject thiz, jint fd, 
        jbyteArray byteArray, jint offset, jint nbytes) {

    DBG("fd=%d", fd);
    jboolean isCopy;
    jbyte *bytes = env->GetByteArrayElements(byteArray, &isCopy);
    jlong result;

    for (;;) {
        result = ::write(fd, (const char *) bytes + offset, (int) nbytes);

        if ((result != -1) || (errno != EINTR)) {
            break;
        }

        /*
         * If we didn't break above, that means that the read() call
         * returned due to EINTR. We shield Java code from this
         * possibility by trying again. Note that this is different
         * from EAGAIN, which should result in this code throwing
         * an InterruptedIOException.
         */
    }

    env->ReleaseByteArrayElements(byteArray, bytes, JNI_ABORT);

    if (result == -1) {
        if (errno == EAGAIN) {
            jniThrowException(env, "java/io/InterruptedIOException",
                    "Write timed out");
        } else {
        jniThrowException(env, "java/io/IOException", strerror(errno));
        }
    }

    return result;
}

static jlong OSFileSystem_readvImpl(JNIEnv *env, jobject thiz, jint fd, 
        jintArray jbuffers, jintArray joffsets, jintArray jlengths, jint size) {

    DBG("fd=%d", fd);
    jboolean bufsCopied = JNI_FALSE;
    jboolean offsetsCopied = JNI_FALSE;
    jboolean lengthsCopied = JNI_FALSE;
    jint *bufs; 
    jint *offsets;
    jint *lengths;
    int i = 0;
    long totalRead = 0;  
    struct iovec *vectors = (struct iovec *)::malloc(size * sizeof(struct iovec));
    if(vectors == NULL) {
        return -1;
    }
    bufs = env->GetIntArrayElements(jbuffers, &bufsCopied);
    offsets = env->GetIntArrayElements(joffsets, &offsetsCopied);
    lengths = env->GetIntArrayElements(jlengths, &lengthsCopied);
    while(i < size) {
        vectors[i].iov_base = (void *)((int)(bufs[i]+offsets[i]));
        vectors[i].iov_len = lengths[i];
        i++;
    }
    totalRead = ::readv(fd, vectors, size);
    if(bufsCopied) {
        env->ReleaseIntArrayElements(jbuffers, bufs, JNI_ABORT);
    }
    if(offsetsCopied) {
        env->ReleaseIntArrayElements(joffsets, offsets, JNI_ABORT);
    }
    if(lengthsCopied) {
        env->ReleaseIntArrayElements(jlengths, lengths, JNI_ABORT);
    }
    free(vectors);
    return totalRead;
}

static jlong OSFileSystem_writevImpl(JNIEnv *env, jobject thiz, jint fd, 
        jintArray jbuffers, jintArray joffsets, jintArray jlengths, jint size) {

    DBG("fd=%d", fd);
    jboolean bufsCopied = JNI_FALSE;
    jboolean offsetsCopied = JNI_FALSE;
    jboolean lengthsCopied = JNI_FALSE;
    jint *bufs; 
    jint *offsets;
    jint *lengths;
    int i = 0;
    long totalRead = 0;  
    struct iovec *vectors = (struct iovec *)::malloc(size * sizeof(struct iovec));
    if(vectors == NULL) {
        return -1;
    }
    bufs = env->GetIntArrayElements(jbuffers, &bufsCopied);
    offsets = env->GetIntArrayElements(joffsets, &offsetsCopied);
    lengths = env->GetIntArrayElements(jlengths, &lengthsCopied);
    while(i < size) {
        vectors[i].iov_base = (void *)((int)(bufs[i]+offsets[i]));
        vectors[i].iov_len = lengths[i];
        i++;
    }
    totalRead = ::writev(fd, vectors, size);
    if(bufsCopied) {
        env->ReleaseIntArrayElements(jbuffers, bufs, JNI_ABORT);
    }
    if(offsetsCopied) {
        env->ReleaseIntArrayElements(joffsets, offsets, JNI_ABORT);
    }
    if(lengthsCopied) {
        env->ReleaseIntArrayElements(jlengths, lengths, JNI_ABORT);
    }
    free(vectors);
    return totalRead;
}

static jint OSFileSystem_closeImpl(JNIEnv * env, jobject thiz, jint fd) {
    jint result;

    DBG("fd=%d", fd);
    for (;;) {
        result = (jint) ::close(fd);

        if ((result != -1) || (errno != EINTR)) {
            break;
        }

        /*
         * If we didn't break above, that means that the close() call
         * returned due to EINTR. We shield Java code from this
         * possibility by trying again.
         */
    }

    return result;
}

static jint OSFileSystem_truncateImpl(JNIEnv * env, jobject thiz, jint fd, 
        jlong size) {

    DBG("fd=%d", fd);

    int rc;
    off_t length = (off_t) size;

    // If file offsets are 32 bit, truncate the newLength to that range
    if(sizeof (off_t) < sizeof (jlong)) {
        if(length > 0x7FFFFFFF) {
            length = 0x7FFFFFFF;
        } else if(length < -0x7FFFFFFF) {
            length = -0x7FFFFFFF;
        }
    }

  rc = ::ftruncate((int)fd, length);

  return (jint) rc;

}

static jint OSFileSystem_openImpl(JNIEnv * env, jobject obj, jbyteArray path, 
        jint jflags) {

    TRACE;
    int flags = 0;
    int mode = 0; 
    jint * portFD;
    jsize length;
    char pathCopy[HyMaxPath];

// BEGIN android-changed
// don't want default permissions to allow global access.
    switch(jflags) {
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
// BEGIN android-changed

    flags = EsTranslateOpenFlags(flags);

    length = env->GetArrayLength (path);
    length = length < HyMaxPath - 1 ? length : HyMaxPath - 1;
    env->GetByteArrayRegion (path, 0, length, (jbyte *)pathCopy);
    pathCopy[length] = '\0';
    convertToPlatform (pathCopy);

    int cc;

    if(pathCopy == NULL) {
        jniThrowException(env, "java/lang/NullPointerException", NULL);
        return -1;
    }

    do {
        cc = ::open(pathCopy, flags, mode);
    } while(cc < 0 && errno == EINTR);

    if(cc < 0 && errno > 0) {
        cc = -errno;
    }

    DBG("return fd=%d", cc);
    return cc;
}

static jlong OSFileSystem_transferImpl(JNIEnv *env, jobject thiz, jint fd, 
        jobject sd, jlong offset, jlong count) {

    DBG("fd=%d", fd);
    int socket;
    off_t off;

    socket = jniGetFDFromFileDescriptor(env, sd);
    if(socket == 0 || socket == -1) {
        return -1;
    }

    /* Value of offset is checked in jint scope (checked in java layer)
       The conversion here is to guarantee no value lost when converting offset to off_t
     */
    off = offset;

    return sendfile(socket,(int)fd,(off_t *)&off,(size_t)count);
}

static jint OSFileSystem_ioctlAvailable(JNIEnv *env, jobject thiz, jint fd) {
    DBG("fd=%d", fd);
    int avail = 0;
    int rc = ::ioctl(fd, FIONREAD, &avail);

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
        jniThrowException(env, "java/io/IOException", strerror(errno));
        avail = 0;
    }  

    return (jint) avail;
}

static jlong OSFileSystem_ttyReadImpl(JNIEnv *env, jobject thiz, 
        jbyteArray byteArray, jint offset, jint nbytes) {
  
    TRACE;
    jboolean isCopy;
    jbyte *bytes = env->GetByteArrayElements(byteArray, &isCopy);
    jlong result;

    for(;;) {

        result = (jlong) ::read(STDIN_FILENO, (char *)(bytes + offset), (int) nbytes);

        if ((result != -1) || (errno != EINTR)) {
            break;
        }

        /*
         * If we didn't break above, that means that the read() call
         * returned due to EINTR. We shield Java code from this
         * possibility by trying again. Note that this is different
         * from EAGAIN, which should result in this code throwing
         * an InterruptedIOException.
         */
    }

    env->ReleaseByteArrayElements(byteArray, bytes, 0);

    if (result == 0) {
        return -1;
    }
    
    if (result == -1) {
        if (errno == EAGAIN) {
            jniThrowException(env, "java/io/InterruptedIOException",
                    "Read timed out");
        } else {
            jniThrowException(env, "java/io/IOException", strerror(errno));
        }
    }

    return result;
}

static JNINativeMethod osfs_methods[] = {
    NATIVE_METHOD(OSFileSystem, lockImpl, "(IJJIZ)I"),
    NATIVE_METHOD(OSFileSystem, getAllocGranularity, "()I"),
    NATIVE_METHOD(OSFileSystem, unlockImpl, "(IJJ)I"),
    NATIVE_METHOD(OSFileSystem, fflushImpl, "(IZ)I"),
    NATIVE_METHOD(OSFileSystem, seekImpl, "(IJI)J"),
    NATIVE_METHOD(OSFileSystem, readDirectImpl, "(IIII)J"),
    NATIVE_METHOD(OSFileSystem, writeDirectImpl, "(IIII)J"),
    NATIVE_METHOD(OSFileSystem, readImpl, "(I[BII)J"),
    NATIVE_METHOD(OSFileSystem, writeImpl, "(I[BII)J"),
    NATIVE_METHOD(OSFileSystem, readvImpl, "(I[I[I[II)J"),
    NATIVE_METHOD(OSFileSystem, writevImpl, "(I[I[I[II)J"),
    NATIVE_METHOD(OSFileSystem, closeImpl, "(I)I"),
    NATIVE_METHOD(OSFileSystem, truncateImpl, "(IJ)I"),
    NATIVE_METHOD(OSFileSystem, openImpl, "([BI)I"),
    NATIVE_METHOD(OSFileSystem, transferImpl, "(ILjava/io/FileDescriptor;JJ)J"),
    NATIVE_METHOD(OSFileSystem, ioctlAvailable, "(I)I"),
    NATIVE_METHOD(OSFileSystem, ttyReadImpl, "([BII)J"),
};

CRYSTAX_LOCAL
bool init(JNIEnv *env)
{
    DBG("trying to register OSFileSystem (eclair implementation) native methods...");
    const char *osfsname = "org/apache/harmony/luni/platform/OSFileSystem";
    if (!register_native_methods(env, osfsname,
            osfs_methods, sizeof(osfs_methods)/sizeof(osfs_methods[0])))
    {
        ERR("can't register OSFileSystem native methods");
        return false;
    }
    DBG("OSFileSystem (eclair implementation) native methods registered");

    return true;
}

} // namespace eclair
} // namespace osfs
} // namespace fileio
} // namespace crystax
