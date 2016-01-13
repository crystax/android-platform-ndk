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

#ifndef _CRYSTAX_JUTILS_METHOD_HPP_881A92D6FDC64BB18193C335A4CBA613
#define _CRYSTAX_JUTILS_METHOD_HPP_881A92D6FDC64BB18193C335A4CBA613

#include <crystax/id.h>

#ifndef __cplusplus
#error "This is C++ header file, you shouldn't include it to non-C++ sources"
#endif

#include <crystax/jutils/jni.hpp>
#include <crystax/jutils/jholder.hpp>
#include <crystax/jutils/exceptions.hpp>
#include <crystax/jutils/helper.hpp>
#include <crystax/jutils/class.hpp>
#include <crystax/jutils/field.hpp>

namespace crystax
{
namespace jni
{

inline
jmethodID get_method_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    auto mid = env->GetMethodID(cls.get(), name, signature);
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return mid;
}

inline
jmethodID get_method_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_method_id(jnienv(), cls, name, signature);
}

template <typename T, typename Refcounter>
jmethodID get_method_id(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_method_id(env, get_class(obj), name, signature);
}

template <typename T, typename Refcounter>
jmethodID get_method_id(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_method_id(jnienv(), obj, name, signature);
}

inline
jmethodID get_static_method_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    auto mid = env->GetStaticMethodID(cls.get(), name, signature);
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return mid;
}

inline
jmethodID get_static_method_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_static_method_id(jnienv(), cls, name, signature);
}

template <typename T, typename Refcounter>
jmethodID get_static_method_id(JNIEnv *env, jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_static_method_id(env, get_class(obj), name, signature);
}

template <typename T, typename Refcounter>
jmethodID get_static_method_id(jholder<T, Refcounter> const &obj, const char *name, const char *signature)
{
    return get_static_method_id(jnienv(), obj, name, signature);
}

// Warning!!! C++11 magic below: variadic templates and perfect forwarding!
// We need to define own implementation of std::forward because we don't have
// standard C++ library when building libcrystax.

namespace details
{

template <typename T>
struct remove_reference
{
    typedef T type;
};

template <typename T>
struct remove_reference<T &>
{
    typedef T type;
};

template <typename T>
struct remove_reference<T &&>
{
    typedef T type;
};

template <typename T>
struct is_lvalue_reference
{
    enum {value = false};
};

template <typename T>
struct is_lvalue_reference<T &>
{
    enum {value = true};
};

template <typename T>
inline T && forward(typename remove_reference<T>::type &arg)
{
    return static_cast<T&&>(arg);
}

template <typename T>
inline T && forward(typename remove_reference<T>::type &&arg)
{
    static_assert(!is_lvalue_reference<T>::value, "template argument is an lvalue reference type");
    return static_cast<T&&>(arg);
}

} // namespace details

template <typename Ret, typename T, typename... Args>
Ret call_method(JNIEnv *env, T const &obj, jmethodID mid, Args&&... args)
{
    return details::jni_helper<Ret>::call_method(env, obj, mid, details::forward<Args>(args)...);
}

template <typename Ret, typename T, typename... Args>
Ret call_method(T const &obj, jmethodID mid, Args&&... args)
{
    return call_method<Ret>(jnienv(), obj, mid, details::forward<Args>(args)...);
}

template <typename Ret, typename... Args>
Ret call_method(JNIEnv *env, const char *clsname, jmethodID mid, Args&&... args)
{
    return call_method<Ret>(env, find_class(clsname), mid, details::forward<Args>(args)...);
}

template <typename Ret, typename... Args>
Ret call_method(const char *clsname, jmethodID mid, Args&&... args)
{
    return call_method<Ret>(jnienv(), clsname, mid, details::forward<Args>(args)...);
}

template <typename Ret, typename T, typename... Args>
Ret call_method(JNIEnv *env, T const &obj, const char *name, const char *signature, Args&&... args)
{
    return call_method<Ret>(env, obj, get_method_id(env, obj, name, signature), details::forward<Args>(args)...);
}

template <typename Ret, typename T, typename... Args>
Ret call_method(T const &obj, const char *name, const char *signature, Args&&... args)
{
    return call_method<Ret>(jnienv(), obj, name, signature, details::forward<Args>(args)...);
}

template <typename Ret, typename... Args>
Ret call_method(JNIEnv *env, const char *clsname, const char *name, const char *signature, Args&&... args)
{
    return call_method<Ret>(env, find_class(clsname), name, signature, details::forward<Args>(args)...);
}

template <typename Ret, typename... Args>
Ret call_method(const char *clsname, const char *name, const char *signature, Args&&... args)
{
    return call_method<Ret>(jnienv(), clsname, name, signature, details::forward<Args>(args)...);
}

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_METHOD_HPP_881A92D6FDC64BB18193C335A4CBA613 */
