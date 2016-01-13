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

#ifndef _CRYSTAX_JUTILS_HELPER_HPP_1E759CAB22894E80AFA1B4CB7131A83F
#define _CRYSTAX_JUTILS_HELPER_HPP_1E759CAB22894E80AFA1B4CB7131A83F

#include <crystax/id.h>

#ifndef __cplusplus
#error "This is C++ header file, you shouldn't include it to non-C++ sources"
#endif

#include <crystax/jutils/jni.hpp>
#include <crystax/jutils/jholder.hpp>

namespace crystax
{
namespace jni
{

namespace details
{

void call_void_method(JNIEnv *env, jclass cls, jmethodID mid, ...);
void call_void_method(JNIEnv *env, jobject obj, jmethodID mid, ...);
#define CRYSTAX_PP_STEP(type) \
    type get_ ## type ## _field(JNIEnv *env, jobject obj, jfieldID fid); \
    type get_ ## type ## _field(JNIEnv *env, jclass cls, jfieldID fid); \
    void set_ ## type ## _field(JNIEnv *env, jobject obj, jfieldID fid, type const &arg); \
    void set_ ## type ## _field(JNIEnv *env, jclass cls, jfieldID fid, type const &arg); \
    type call_ ## type ## _method(JNIEnv *env, jclass cls, jmethodID mid, ...); \
    type call_ ## type ## _method(JNIEnv *env, jobject obj, jmethodID mid, ...);
#include <crystax/details/jni.inc>
#undef CRYSTAX_PP_STEP

#define CRYSTAX_PP_STEP(type) inline type raw_arg(type arg) {return arg;}
CRYSTAX_PP_STEP(jboolean)
CRYSTAX_PP_STEP(jbyte)
CRYSTAX_PP_STEP(jchar)
CRYSTAX_PP_STEP(jshort)
CRYSTAX_PP_STEP(jint)
CRYSTAX_PP_STEP(jlong)
CRYSTAX_PP_STEP(jfloat)
CRYSTAX_PP_STEP(jdouble)
CRYSTAX_PP_STEP(jobject)
CRYSTAX_PP_STEP(jclass)
CRYSTAX_PP_STEP(jstring)
CRYSTAX_PP_STEP(jthrowable)
CRYSTAX_PP_STEP(jarray)
CRYSTAX_PP_STEP(jbooleanArray)
CRYSTAX_PP_STEP(jbyteArray)
CRYSTAX_PP_STEP(jshortArray)
CRYSTAX_PP_STEP(jintArray)
CRYSTAX_PP_STEP(jlongArray)
CRYSTAX_PP_STEP(jfloatArray)
CRYSTAX_PP_STEP(jdoubleArray)
CRYSTAX_PP_STEP(jobjectArray)
#undef CRYSTAX_PP_STEP

template <typename T, typename Refcounter>
T raw_arg(jholder<T, Refcounter> const &arg)
{
    return arg.get();
}

template <typename Ret>
struct jni_helper;

template <>
struct jni_helper<void>
{
    template <typename T, typename... Args>
    static void call_method(JNIEnv *env, T const &obj, jmethodID mid, Args&&... args)
    {
        details::call_void_method(env, raw_arg(obj), mid, raw_arg(args)...);
    }
};

#define CRYSTAX_PP_STEP(type) \
template <> \
struct jni_helper<type> \
{ \
    template <typename T> \
    static type get_field(JNIEnv *env, T const &obj, jfieldID fid) \
    { \
        return details::get_ ## type ## _field(env, raw_arg(obj), fid); \
    } \
    template <typename T> \
    static void set_field(JNIEnv *env, T const &obj, jfieldID fid, type const &arg) \
    { \
        details::set_ ## type ## _field(env, raw_arg(obj), fid, arg); \
    } \
    template <typename T, typename... Args> \
    static type call_method(JNIEnv *env, T const &obj, jmethodID mid, Args&&... args) \
    { \
        return details::call_ ## type ## _method(env, raw_arg(obj), mid, raw_arg(args)...); \
    } \
};
#include <crystax/details/jni.inc>
#undef CRYSTAX_PP_STEP

template <typename T>
struct jni_signature
{
    static const char *signature;
};

} // namespace details

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_HELPER_HPP_1E759CAB22894E80AFA1B4CB7131A83F */
