/*
 * Copyright (c) 2011-2015 CrystaX.
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
#include <pthread.h>
#include <jni.h>
#include <dlfcn.h>
#include "crystax/private.h"

static pthread_mutex_t mtx = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
static void *libart_handle = NULL;

typedef jint (*JNI_GetDefaultJavaVMInitArgs_func_t)(void *);
typedef jint (*JNI_CreateJavaVM_func_t)(JavaVM**, JNIEnv**, void*);
typedef jint (*JNI_GetCreatedJavaVMs_func_t)(JavaVM**, jsize, jsize*);

static JNI_GetDefaultJavaVMInitArgs_func_t JNI_GetDefaultJavaVMInitArgs_func = NULL;
static JNI_CreateJavaVM_func_t JNI_CreateJavaVM_func = NULL;
static JNI_GetCreatedJavaVMs_func_t JNI_GetCreatedJavaVMs_func = NULL;

CRYSTAX_GLOBAL
jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    FRAME_TRACER;
    return crystax_jni_on_load(vm);
}

CRYSTAX_GLOBAL
void JNI_OnUnload(JavaVM* vm, void* /*reserved*/)
{
    FRAME_TRACER;
    crystax_jni_on_unload(vm);
}

static void initialize_jni_runtime()
{
    FRAME_TRACER;

    if (::pthread_mutex_lock(&mtx) != 0)
        PANIC("Can't lock mutex");

    TRACE;
    if (!libart_handle)
    {
        TRACE;
        libart_handle = ::dlopen("libart.so", RTLD_NOW);
        if (!libart_handle)
            PANIC("Can't open libart.so");

        TRACE;
        JNI_GetDefaultJavaVMInitArgs_func = (JNI_GetDefaultJavaVMInitArgs_func_t)::dlsym(
                libart_handle, "JNI_GetDefaultJavaVMInitArgs");
        if (!JNI_GetDefaultJavaVMInitArgs_func)
            PANIC("Can't find JNI_GetDefaultJavaVMInitArgs in libart.so");

        TRACE;
        JNI_CreateJavaVM_func = (JNI_CreateJavaVM_func_t)::dlsym(
                libart_handle, "JNI_CreateJavaVM");
        if (!JNI_CreateJavaVM_func)
            PANIC("Can't find JNI_CreateJavaVM in libart.so");

        TRACE;
        JNI_GetCreatedJavaVMs_func = (JNI_GetCreatedJavaVMs_func_t)::dlsym(
                libart_handle, "JNI_GetCreatedJavaVMs");
        if (!JNI_GetCreatedJavaVMs_func)
            PANIC("Can't find JNI_GetCreatedJavaVMs in libart.so");
    }

    TRACE;
    if (::pthread_mutex_unlock(&mtx) != 0)
        PANIC("Can't unlock mutex");
}

CRYSTAX_GLOBAL
jint JNI_GetDefaultJavaVMInitArgs(void* args)
{
    initialize_jni_runtime();
    return JNI_GetDefaultJavaVMInitArgs_func(args);
}

CRYSTAX_GLOBAL
jint JNI_CreateJavaVM(JavaVM** vm, JNIEnv** env, void* args)
{
    initialize_jni_runtime();
    return JNI_CreateJavaVM(vm, env, args);
}

CRYSTAX_GLOBAL
jint JNI_GetCreatedJavaVMs(JavaVM** vms, jsize size, jsize* count)
{
    initialize_jni_runtime();
    return JNI_GetCreatedJavaVMs(vms, size, count);
}
