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

namespace fileio
{
namespace osfs
{
bool init(JNIEnv *env);
} // namespace osfs
} // namespace fileio

static JavaVM *s_jvm = NULL;
static pthread_key_t s_jnienv_key;
static pthread_once_t s_jnienv_key_once = PTHREAD_ONCE_INIT;

namespace jni
{

JavaVM *jvm()
{
    return s_jvm;
}

JNIEnv *jnienv()
{
    return reinterpret_cast<JNIEnv *>(::pthread_getspecific(s_jnienv_key));
}

} // namespace jni

static bool save_jnienv(JNIEnv *env)
{
    if (::pthread_setspecific(s_jnienv_key, env) != 0)
        return false;
    return true;
}

struct pthread_func_arg_t
{
    void *(*func)(void *);
    void *arg;
};

CRYSTAX_LOCAL
void *thread_func(void *a)
{
    TRACE;
    pthread_func_arg_t *parg = (pthread_func_arg_t*)a;
    void *(*func)(void *) = parg->func;
    void *arg = parg->arg;
    free(parg);

    JavaVM *vm = jni::jvm();
    if (vm)
    {
        JNIEnv *env;
        vm->AttachCurrentThread(&env, NULL);
        if (!save_jnienv(env))
            ::abort();
    }

    void *result = func(arg);

    if (vm)
        vm->DetachCurrentThread();

    return result;
}

} // namespace crystax

CRYSTAX_GLOBAL
int pthread_create(pthread_t *pth, pthread_attr_t const *pattr, void * (*func)(void *), void *arg)
{
    TRACE;
    crystax::pthread_func_arg_t *parg = (crystax::pthread_func_arg_t*)malloc(sizeof(crystax::pthread_func_arg_t));
    parg->func = func;
    parg->arg = arg;
    return system_pthread_create(pth, pattr, &crystax::thread_func, parg);
}

static bool __crystax_init()
{
#define NEXT_MODULE_INIT(x) \
    DBG("initialize " #x); \
    if (__crystax_ ## x ## _init() < 0) \
    { \
        DBG(#x " initialization failed"); \
        return false; \
    }

    NEXT_MODULE_INIT(fileio);
    NEXT_MODULE_INIT(locale);

#undef NEXT_MODULE_INIT

    return true;
}

CRYSTAX_GLOBAL __attribute__((constructor))
void __crystax_on_load()
{
    TRACE;

    if (::pthread_key_create(&::crystax::s_jnienv_key, NULL) != 0)
        ::abort();

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
    ::pthread_key_delete(crystax::s_jnienv_key);
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
    ::crystax::save_jnienv(env);
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
    if (!::crystax::save_jnienv(env))
    {
        ERR("can't save jnienv");
        return -1;
    }

    TRACE;
    if (!::crystax::fileio::osfs::init(env))
    {
        ERR("can't init osfs");
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
