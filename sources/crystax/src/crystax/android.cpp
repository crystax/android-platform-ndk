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

#if defined(CRYSTAX_INIT_DEBUG) && CRYSTAX_INIT_DEBUG == 1
#ifdef CRYSTAX_DEBUG
#undef CRYSTAX_DEBUG
#endif
#define CRYSTAX_DEBUG 1
#endif

#include "crystax/private.h"

namespace crystax
{

static JavaVM *s_jvm = NULL;
static pthread_key_t s_jnienv_key;
static pthread_once_t s_jnienv_key_once = PTHREAD_ONCE_INIT;

namespace jni
{

JavaVM *jvm()
{
    return s_jvm;
}

static void jnienv_detach_thread(void *arg)
{
    JNIEnv *env = reinterpret_cast<JNIEnv *>(arg);
    DBG("env=%p, jvm=%p", env, jvm());
    if (jvm())
        jvm()->DetachCurrentThread();
}

static void jnienv_key_create()
{
    TRACE;
    if (::pthread_key_create(&s_jnienv_key, &jnienv_detach_thread) != 0)
        ::abort();
}

static bool save_jnienv(JNIEnv *env)
{
    TRACE;

    ::pthread_once(&s_jnienv_key_once, &jnienv_key_create);

    if (::pthread_setspecific(s_jnienv_key, env) != 0)
        return false;
    return true;
}

JNIEnv *jnienv()
{
    ::pthread_once(&s_jnienv_key_once, &jnienv_key_create);

    JNIEnv *env = reinterpret_cast<JNIEnv *>(::pthread_getspecific(s_jnienv_key));
    if (!env && jni::jvm())
    {
        DBG("JNIEnv was not yet set for this thread, do it now");
        jni::jvm()->AttachCurrentThread(&env, NULL);
        if (!save_jnienv(env))
            ::abort();
    }
    return env;
}

} // namespace jni

} // namespace crystax

static bool __crystax_init()
{
#define NEXT_MODULE_INIT(x) \
    DBG("initialize " #x); \
    if (__crystax_ ## x ## _init() < 0) \
    { \
        ERR(#x " initialization failed"); \
        return false; \
    }

    NEXT_MODULE_INIT(locale);

#undef NEXT_MODULE_INIT

    return true;
}

CRYSTAX_GLOBAL __attribute__((constructor))
void __crystax_on_load()
{
    TRACE;

    ::pthread_once(&::crystax::s_jnienv_key_once, &::crystax::jni::jnienv_key_create);

    if (!__crystax_init())
    {
        __android_log_print(ANDROID_LOG_FATAL, "CRYSTAX", "libcrystax initialization failed");
        ::abort();
    }

    TRACE;
}

CRYSTAX_GLOBAL __attribute__((destructor))
void __crystax_on_unload()
{
    TRACE;
    ::pthread_key_delete(::crystax::s_jnienv_key);
    TRACE;
}

CRYSTAX_GLOBAL
JavaVM *crystax_jvm()
{
    return ::crystax::jni::jvm();
}

CRYSTAX_GLOBAL
JNIEnv *crystax_jnienv()
{
    return ::crystax::jni::jnienv();
}

CRYSTAX_GLOBAL
void crystax_save_jnienv(JNIEnv *env)
{
    ::crystax::jni::save_jnienv(env);
}

CRYSTAX_GLOBAL
jint crystax_jni_on_load(JavaVM *vm)
{
    jint jversion = JNI_VERSION_1_4;
    JNIEnv *env;

    TRACE;
    if (vm->GetEnv((void**)&env, jversion) != JNI_OK)
    {
        ERR("can't get env from JVM");
        return -1;
    }

    TRACE;
    ::crystax::s_jvm = vm;
    if (!::crystax::jni::save_jnienv(env))
    {
        ERR("can't save jnienv");
        return -1;
    }

    TRACE;
    return jversion;
}

CRYSTAX_GLOBAL
void crystax_jni_on_unload(JavaVM * /* vm */)
{
    TRACE;
    ::crystax::s_jvm = NULL;
}
