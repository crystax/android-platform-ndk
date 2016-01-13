/*
 * Copyright (c) 2011-2016 CrystaX.
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
 * THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX.
 */

#include <crystax.h>
#include <crystax/jutils.hpp>
#include <crystax/log.h>
#include <crystax/memory.hpp>

#include <errno.h>
#include <pthread.h>
#include <string.h>

namespace crystax
{
namespace jni
{

static bool jcast_initialized = false;
static pthread_mutex_t jcast_mtx = PTHREAD_MUTEX_INITIALIZER;

static jclass clsString;
static jmethodID midStringCtor;

static void jcast_init()
{
    int rc;
    if ((rc = ::pthread_mutex_lock(&jcast_mtx)) != 0)
        CRYSTAX_PANIC("Can't lock jcast mutex: %s", ::strerror(rc));

    if (!jcast_initialized)
    {
        JNIEnv *env = jnienv();
        jhclass cls(env->FindClass("java/lang/String"));
        if (!cls) CRYSTAX_PANIC("Can't find java/lang/String");

        clsString = (jclass)env->NewGlobalRef(cls.get());
        if (!clsString) CRYSTAX_PANIC("Can't make new global ref for java/lang/String");

        midStringCtor = env->GetMethodID(clsString, "<init>", "([BLjava/lang/String;)V");
        if (!midStringCtor) CRYSTAX_PANIC("Can't find constructor for java/lang/String");

        jcast_initialized = true;
    }

    if ((rc = ::pthread_mutex_unlock(&jcast_mtx)) != 0)
        CRYSTAX_PANIC("Can't unlock jcast mutex: %s", ::strerror(rc));
}

const char * jcaster<const char *, jstring>::operator()(jstring v)
{
    jcast_init();

    JNIEnv *env = jnienv();
    const char *s = env->GetStringUTFChars(v, JNI_FALSE);
    const char *ret = ::strdup(s);
    env->ReleaseStringUTFChars(v, s);
    return ret;
}

jhstring jcaster<jhstring, const char *>::operator()(const char *s)
{
    jcast_init();

    if (!s) return jhstring();

    JNIEnv *env = jnienv();

    jhstring encoding(env->NewStringUTF("UTF-8"));
    if (!encoding) rethrow(jexception(env));

    size_t slen = ::strlen(s);
    jhbyteArray bytes(env->NewByteArray(slen));
    if (!bytes) rethrow(jexception(env));

    env->SetByteArrayRegion(bytes.get(), 0, slen, reinterpret_cast<jbyte const *>(s));

    return jhstring((jstring)env->NewObject(clsString, midStringCtor, bytes.get(), encoding.get()));
}

} // namespace jni
} // namespace crystax
