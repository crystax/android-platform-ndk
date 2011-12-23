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

#ifndef _CRYSTAX_JUTILS_HPP_e6ec1ce9928d459eae2faf1df240509a
#define _CRYSTAX_JUTILS_HPP_e6ec1ce9928d459eae2faf1df240509a

#include <jni.h>

namespace crystax
{
namespace jni
{

JavaVM *jvm();
JNIEnv *jnienv();

template <typename T>
class jholder
{
    typedef void (jholder::*safe_bool_type)();
    void non_null_object() {}

public:
    explicit jholder(T obj = 0) :object(obj) {}

    jholder(jholder const &c)
        :object(c.object ? (T)jnienv()->NewLocalRef(c.object) : 0)
    {}

    template <typename U>
    explicit jholder(jholder<U> const &c)
        :object(c.object ? (T)jnienv()->NewLocalRef(c.object) : 0)
    {}

    ~jholder()
    {
        if (object)
            jnienv()->DeleteLocalRef(object);
    }

    jholder &operator=(jholder const &c)
    {
        jholder tmp(c);
        swap(tmp);
        return *this;
    }

    void reset(T obj = 0)
    {
        if (object)
            jnienv()->DeleteLocalRef(object);
        object = obj;
    }

    T get() const {return object;}

    T release()
    {
        T ret = object;
        object = 0;
        return ret;
    }

    bool operator!() const {return !object;}
    operator safe_bool_type () const
    {
        return object ? &jholder::non_null_object : 0;
    }

private:
    void swap(jholder &c)
    {
        T tmp = object;
        object = c.object;
        c.object = tmp;
    }

private:
    T object;
};

typedef jholder<jclass> jhclass;
typedef jholder<jobject> jhobject;
typedef jholder<jstring> jhstring;
typedef jholder<jthrowable> jhthrowable;
typedef jholder<jarray> jharray;
typedef jholder<jbyteArray> jhbyteArray;
typedef jholder<jbooleanArray> jhbooleanArray;
typedef jholder<jcharArray> jhcharArray;
typedef jholder<jshortArray> jhshortArray;
typedef jholder<jintArray> jhintArray;
typedef jholder<jlongArray> jhlongArray;
typedef jholder<jfloatArray> jhfloatArray;
typedef jholder<jdoubleArray> jhdoubleArray;
typedef jholder<jobjectArray> jhobjectArray;

namespace details
{

template <typename T, typename U>
struct jcast_helper;

template <>
struct jcast_helper<const char *, jstring>
{
    static const char *cast(jstring const &v);
};

template <>
struct jcast_helper<jhstring, const char *>
{
    static jhstring cast(const char *s);
};

template <>
struct jcast_helper<const char *, jhstring>
{
    static const char *cast(jhstring const &v)
    {
        return jcast_helper<const char *, jstring>::cast(v.get());
    }
};

} // namespace details

template <typename T, typename U>
T jcast(U const &v)
{
    return details::jcast_helper<T, U>::cast(v);
}

inline
jhclass find_class(JNIEnv *env, const char *clsname)
{
    return jhclass((jclass)env->FindClass(clsname));
}

inline
jhclass find_class(const char *clsname)
{
    return find_class(jnienv(), clsname);
}

template <typename T>
jhclass get_class(JNIEnv *env, jholder<T> const &obj)
{
    return jhclass(env->GetObjectClass(obj.get()));
}

template <typename T>
jhclass get_class(jholder<T> const &obj)
{
    return get_class(jnienv(), obj);
}

inline
jmethodID get_method_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    return env->GetMethodID(cls.get(), name, signature);
}

inline
jmethodID get_method_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_method_id(jnienv(), cls, name, signature);
}

template <typename T>
jmethodID get_method_id(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_method_id(env, get_class(obj), name, signature);
}

template <typename T>
jmethodID get_method_id(jholder<T> const &obj, const char *name, const char *signature)
{
    return get_method_id(jnienv(), obj, name, signature);
}

inline
jmethodID get_static_method_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    return env->GetStaticMethodID(cls.get(), name, signature);
}

inline
jmethodID get_static_method_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_static_method_id(jnienv(), cls, name, signature);
}

template <typename T>
jmethodID get_static_method_id(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_static_method_id(env, get_class(obj), name, signature);
}

template <typename T>
jmethodID get_static_method_id(jholder<T> const &obj, const char *name, const char *signature)
{
    return get_static_method_id(jnienv(), obj, name, signature);
}

inline
jfieldID get_field_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    return env->GetFieldID(cls.get(), name, signature);
}

inline
jfieldID get_field_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_field_id(jnienv(), cls, name, signature);
}

template <typename T>
jfieldID get_field_id(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_field_id(env, get_class(obj), name, signature);
}

template <typename T>
jfieldID get_field_id(jholder<T> const &obj, const char *name, const char *signature)
{
    return get_field_id(jnienv(), obj, name, signature);
}

inline
jfieldID get_static_field_id(JNIEnv *env, jhclass const &cls, const char *name, const char *signature)
{
    return env->GetStaticFieldID(cls.get(), name, signature);
}

inline
jfieldID get_static_field_id(jhclass const &cls, const char *name, const char *signature)
{
    return get_static_field_id(jnienv(), cls, name, signature);
}

template <typename T>
jfieldID get_static_field_id(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_static_field_id(env, get_class(obj), name, signature);
}

template <typename T>
jfieldID get_static_field_id(jholder<T> const &obj, const char *name, const char *signature)
{
    return get_static_field_id(jnienv(), obj, name, signature);
}

inline void jthrow(JNIEnv *env, jhthrowable const &ex) {env->Throw(ex.get());}
inline void jthrow(jhthrowable const &ex) {jthrow(jnienv(), ex);}

inline void jthrow(JNIEnv *env, jhclass const &cls, char const *what) {env->ThrowNew(cls.get(), what);}
inline void jthrow(jhclass const &cls, char const *what) {jthrow(jnienv(), cls, what);}

inline void jthrow(JNIEnv *env, char const *clsname, char const *what) {jthrow(find_class(env, clsname), what);}
inline void jthrow(char const *clsname, char const *what) {jthrow(jnienv(), clsname, what);}

bool jexcheck(JNIEnv *env);
inline bool jexcheck() {return jexcheck(jnienv());}

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

template <typename T>
T raw_arg(jholder<T> const &arg)
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

// Get field

template <typename Ret, typename T>
Ret get_field(JNIEnv *env, jholder<T> const &obj, jfieldID fid)
{
    return details::jni_helper<Ret>::get_field(env, obj, fid);
}

template <typename Ret, typename T>
Ret get_field(jholder<T> const &obj, jfieldID fid)
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

template <typename Ret, typename T>
Ret get_field(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_field<Ret>(env, obj, get_field_id(env, obj, name, signature));
}

template <typename Ret, typename T>
Ret get_field(jholder<T> const &obj, const char *name, const char *signature)
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

template <typename Ret, typename T>
Ret get_field(JNIEnv *env, jholder<T> const &obj, const char *name)
{
    return get_field<Ret>(env, obj, name, details::jni_signature<Ret>::signature);
}

template <typename Ret, typename T>
Ret get_field(jholder<T> const &obj, const char *name)
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

template <typename Ret, typename T>
Ret get_static_field(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature)
{
    return get_field<Ret>(env, obj, get_static_field_id(env, obj, name, signature));
}

template <typename Ret, typename T>
Ret get_static_field(jholder<T> const &obj, const char *name, const char *signature)
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

template <typename Ret, typename T>
Ret get_static_field(JNIEnv *env, jholder<T> const &obj, const char *name)
{
    return get_static_field<Ret>(env, obj, name, details::jni_signature<Ret>::signature);
}

template <typename Ret, typename T>
Ret get_static_field(jholder<T> const &obj, const char *name)
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

template <typename T, typename Arg>
void set_field(JNIEnv *env, jholder<T> const &obj, jfieldID fid, Arg const &arg)
{
    details::jni_helper<Arg>::set_field(env, obj, fid, arg);
}

template <typename T, typename Arg>
void set_field(jholder<T> const &obj, jfieldID fid, Arg const &arg)
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

template <typename T, typename Arg>
void set_field(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_field(env, obj, get_field_id(env, obj, name, signature), arg);
}

template <typename T, typename Arg>
void set_field(jholder<T> const &obj, const char *name, const char *signature, Arg const &arg)
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

template <typename T, typename Arg>
void set_field(JNIEnv *env, jholder<T> const &obj, const char *name, Arg const &arg)
{
    set_field(env, obj, name, details::jni_signature<Arg>::signature, arg);
}

template <typename T, typename Arg>
void set_field(jholder<T> const &obj, const char *name, Arg const &arg)
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

template <typename T, typename Arg>
void set_static_field(JNIEnv *env, jholder<T> const &obj, const char *name, const char *signature, Arg const &arg)
{
    set_field(env, obj, get_static_field_id(env, obj, name, signature), arg);
}

template <typename T, typename Arg>
void set_static_field(jholder<T> const &obj, const char *name, const char *signature, Arg const &arg)
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

template <typename T, typename Arg>
void set_static_field(JNIEnv *env, jholder<T> const &obj, const char *name, Arg const &arg)
{
    set_static_field(env, obj, name, details::jni_signature<Arg>::signature, arg);
}

template <typename T, typename Arg>
void set_static_field(jholder<T> const &obj, const char *name, Arg const &arg)
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

// Warning!!! C++11 magic below: variadic templates and perfect forwarding!
// We need to define own implementation of std::forward because we don't have
// standard C++ library when building libcrystax.

namespace details
{

#if __GNUC__ == 4 && __GNUC_MINOR__ == 4

template <typename T>
struct identity {typedef T type;};

template <typename T>
inline T && forward(typename identity<T>::type && arg)
{
    return arg;
}

#else // __GNUC__ == 4 && __GNUC_MINOR__ == 4

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
#endif // __GNUC__ == 4 && __GNUC_MINOR__ == 4

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

#endif // _CRYSTAX_JUTILS_HPP_e6ec1ce9928d459eae2faf1df240509a
