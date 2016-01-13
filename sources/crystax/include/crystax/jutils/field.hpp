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

#ifndef _CRYSTAX_JUTILS_FIELD_HPP_FFB5C8AED2C64F52842E9F11F3E814E6
#define _CRYSTAX_JUTILS_FIELD_HPP_FFB5C8AED2C64F52842E9F11F3E814E6

#include <crystax/id.h>

#ifndef __cplusplus
#error "This is C++ header file, you shouldn't include it to non-C++ sources"
#endif

#include <crystax/jutils/jni.hpp>
#include <crystax/jutils/jholder.hpp>
#include <crystax/jutils/exceptions.hpp>
#include <crystax/jutils/class.hpp>
#include <crystax/jutils/helper.hpp>

namespace crystax
{
namespace jni
{


// Get field ID

inline
jfieldID get_field_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    auto fid = env->GetFieldID(cls.get(), name, signature);
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return fid;
}

inline
jfieldID get_field_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_field_id(jnienv(), cls, name, signature);
}

template <typename T, typename Refcounter>
jfieldID get_field_id(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_field_id(env, get_class(obj), name, signature);
}

template <typename T, typename Refcounter>
jfieldID get_field_id(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_field_id(jnienv(), obj, name, signature);
}

// Get static field ID

inline
jfieldID get_static_field_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    auto fid = env->GetStaticFieldID(cls.get(), name, signature);
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return fid;
}

inline
jfieldID get_static_field_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_static_field_id(jnienv(), cls, name, signature);
}

template <typename T, typename Refcounter>
jfieldID get_static_field_id(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_static_field_id(env, get_class(obj), name, signature);
}

template <typename T, typename Refcounter>
jfieldID get_static_field_id(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_static_field_id(jnienv(), obj, name, signature);
}

// Get field

template <typename Ret, typename T, typename Refcounter>
Ret get_field(JNIEnv *env, jholder<T, Refcounter> const &obj, jfieldID fid)
{
    return details::jni_helper<Ret>::get_field(env, obj, fid);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_field(jholder<T, Refcounter> const &obj, jfieldID fid)
{
    return get_field<Ret>(jnienv(), obj, fid);
}

template <typename Ret>
Ret get_field(JNIEnv *env, const char *clsname, jfieldID fid)
{
    return get_field<Ret>(env, find_class(env, clsname), fid);
}

template <typename Ret>
Ret get_field(const char *clsname, jfieldID fid)
{
    return get_field<Ret>(jnienv(), clsname, fid);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_field<Ret>(env, obj, get_field_id(env, obj, name, signature));
}

template <typename Ret, typename T, typename Refcounter>
Ret get_field(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_field<Ret>(jnienv(), obj, name, signature);
}

template <typename Ret>
Ret get_field(JNIEnv *env, const char *clsname, const char *name, const char *signature)
{
    return get_field<Ret>(env, find_class(clsname), name, signature);
}

template <typename Ret>
Ret get_field(const char *clsname, const char *name, const char *signature)
{
    return get_field<Ret>(jnienv(), clsname, name, signature);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name)
{
    return get_field<Ret>(env, obj, name, details::jni_signature<Ret>::signature);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_field(jholder<T, Refcounter> const &obj, const char *name)
{
    return get_field<Ret>(jnienv(), obj, name);
}

template <typename Ret>
Ret get_field(JNIEnv *env, const char *clsname, const char *name)
{
    return get_field<Ret>(env, find_class(env, clsname), name);
}

template <typename Ret>
Ret get_field(const char *clsname, const char *name)
{
    return get_field<Ret>(jnienv(), clsname, name);
}

// Get static field

template <typename Ret, typename T, typename Refcounter>
Ret get_static_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_field<Ret>(env, obj, get_static_field_id(env, obj, name, signature));
}

template <typename Ret, typename T, typename Refcounter>
Ret get_static_field(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_static_field<Ret>(jnienv(), obj, name, signature);
}

template <typename Ret>
Ret get_static_field(JNIEnv *env, const char *clsname, const char *name, const char *signature)
{
    return get_static_field<Ret>(env, find_class(env, clsname), name, signature);
}

template <typename Ret>
Ret get_static_field(const char *clsname, const char *name, const char *signature)
{
    return get_static_field<Ret>(jnienv(), clsname, name, signature);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_static_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name)
{
    return get_static_field<Ret>(env, obj, name, details::jni_signature<Ret>::signature);
}

template <typename Ret, typename T, typename Refcounter>
Ret get_static_field(jholder<T, Refcounter> const &obj, const char *name)
{
    return get_static_field<Ret>(jnienv(), obj, name);
}

template <typename Ret>
Ret get_static_field(JNIEnv *env, const char *clsname, const char *name)
{
    return get_static_field<Ret>(env, find_class(env, clsname), name);
}

template <typename Ret>
Ret get_static_field(const char *clsname, const char *name)
{
    return get_static_field<Ret>(jnienv(), clsname, name);
}

// Set field

template <typename T, typename Refcounter, typename Arg>
void set_field(JNIEnv *env, jholder<T, Refcounter> const &obj, jfieldID fid, Arg const &arg)
{
    details::jni_helper<Arg>::set_field(env, obj, fid, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_field(jholder<T, Refcounter> const &obj, jfieldID fid, Arg const &arg)
{
    set_field(jnienv(), obj, fid, arg);
}

template <typename Arg>
void set_field(JNIEnv *env, const char *clsname, jfieldID fid, Arg const &arg)
{
    set_field(env, find_class(clsname), fid, arg);
}

template <typename Arg>
void set_field(const char *clsname, jfieldID fid, Arg const &arg)
{
    set_field(jnienv(), clsname, fid, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_field(env, obj, get_field_id(env, obj, name, signature), arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_field(jholder<T, Refcounter> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_field(jnienv(), obj, name, signature, arg);
}

template <typename Arg>
void set_field(JNIEnv *env, const char *clsname, const char *name, const char *signature, Arg const &arg)
{
    set_field(env, find_class(clsname), name, signature, arg);
}

template <typename Arg>
void set_field(const char *clsname, const char *name, const char *signature, Arg const &arg)
{
    set_field(jnienv(), clsname, name, signature, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, Arg const &arg)
{
    set_field(env, obj, name, details::jni_signature<Arg>::signature, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_field(jholder<T, Refcounter> const &obj, const char *name, Arg const &arg)
{
    set_field(jnienv(), obj, name, arg);
}

template <typename Arg>
void set_field(JNIEnv *env, const char *clsname, const char *name, Arg const &arg)
{
    set_field(env, find_class(clsname), name, arg);
}

template <typename Arg>
void set_field(const char *clsname, const char *name, Arg const &arg)
{
    set_field(jnienv(), clsname, name, arg);
}

// Set static field

template <typename T, typename Refcounter, typename Arg>
void set_static_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_field(env, obj, get_static_field_id(env, obj, name, signature), arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_static_field(jholder<T, Refcounter> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_static_field(jnienv(), obj, name, signature, arg);
}

template <typename Arg>
void set_static_field(JNIEnv *env, const char *clsname, const char *name, const char *signature, Arg const &arg)
{
    set_static_field(env, find_class(env, clsname), name, signature, arg);
}

template <typename Arg>
void set_static_field(const char *clsname, const char *name, const char *signature, Arg const &arg)
{
    set_static_field(jnienv(), clsname, name, signature, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_static_field(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, Arg const &arg)
{
    set_static_field(env, obj, name, details::jni_signature<Arg>::signature, arg);
}

template <typename T, typename Refcounter, typename Arg>
void set_static_field(jholder<T, Refcounter> const &obj, const char *name, Arg const &arg)
{
    set_static_field(jnienv(), obj, name, arg);
}

template <typename Arg>
void set_static_field(JNIEnv *env, const char *clsname, const char *name, Arg const &arg)
{
    set_static_field(env, find_class(clsname), name, arg);
}

template <typename Arg>
void set_static_field(const char *clsname, const char *name, Arg const &arg)
{
    set_static_field(jnienv(), clsname, name, arg);
}

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_FIELD_HPP_FFB5C8AED2C64F52842E9F11F3E814E6 */
