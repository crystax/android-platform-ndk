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

#ifndef _CRYSTAX_JUTILS_JHOLDER_HPP_AC6864CD125E46BF86F47EF5DE234D31
#define _CRYSTAX_JUTILS_JHOLDER_HPP_AC6864CD125E46BF86F47EF5DE234D31

#include <crystax/id.h>

#ifndef __cplusplus
#error "This is C++ header file, you shouldn't include it to non-C++ sources"
#endif

#include <jni.h>

#include <crystax/jutils/jni.hpp>

namespace crystax
{
namespace jni
{

namespace refcounter
{

struct local final
{
    static jobject newref(JNIEnv *env, jobject obj)
    {
        return obj ? env->NewLocalRef(obj) : 0;
    }

    static void release(JNIEnv *env, jobject obj)
    {
        if (obj) env->DeleteLocalRef(obj);
    }
};

struct global final
{
    static jobject newref(JNIEnv *env, jobject obj)
    {
        return obj ? env->NewGlobalRef(obj) : 0;
    }

    static void release(JNIEnv *env, jobject obj)
    {
        if (obj) env->DeleteGlobalRef(obj);
    }
};

} // namespace refcounter

template <typename T, typename Refcounter = refcounter::local>
class jholder
{
    typedef void (jholder::*safe_bool_type)();
    void non_null_object() {}

public:
    explicit jholder(T obj = 0)
        :object(obj)
    {}

    jholder(jholder const &c)
        :object((T)Refcounter::newref(jnienv(), c.object))
    {}

    template <typename U, typename Refcounter2>
    jholder(jholder<U, Refcounter2> const &c)
        :object((T)Refcounter::newref(jnienv(), c.object))
    {}

    ~jholder()
    {
        Refcounter::release(jnienv(), object);
    }

    jholder &operator=(jholder const &c)
    {
        jholder tmp(c);
        swap(tmp);
        return *this;
    }

    template <typename U, typename Refcounter2>
    jholder &operator=(jholder<U, Refcounter2> const &c)
    {
        jholder tmp(c);
        swap(tmp);
        return *this;
    }

    void reset(T obj = 0)
    {
        Refcounter::release(jnienv(), obj);
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

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_JHOLDER_HPP_AC6864CD125E46BF86F47EF5DE234D31 */
