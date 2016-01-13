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

#ifndef _CRYSTAX_JUTILS_EXCEPTIONS_HPP_5302DB654A1C4F998698D3AE018E46A3
#define _CRYSTAX_JUTILS_EXCEPTIONS_HPP_5302DB654A1C4F998698D3AE018E46A3

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

jhclass find_class(JNIEnv *env, const char *clsname);

inline void jthrow(JNIEnv *env, jhthrowable const &ex)
{
    env->Throw(ex.get());
}

inline void jthrow(jhthrowable const &ex)
{
    jthrow(jnienv(), ex);
}

inline void jthrow(JNIEnv *env, jhclass const &cls, char const *what)
{
    env->ThrowNew(cls.get(), what);
}

inline void jthrow(jhclass const &cls, char const *what)
{
    jthrow(jnienv(), cls, what);
}

inline void jthrow(JNIEnv *env, char const *clsname, char const *what)
{
    jthrow(find_class(env, clsname), what);
}

inline void jthrow(char const *clsname, char const *what)
{
    jthrow(jnienv(), clsname, what);
}

inline void jthrow(char const *what)
{
    jthrow("java/lang/RuntimeException", what);
}

bool jexcheck(JNIEnv *env);
inline bool jexcheck()
{
    return jexcheck(jnienv());
}

inline jhthrowable jexception(JNIEnv *env)
{
    jhthrowable jex(env->ExceptionOccurred());
    if (jex) env->ExceptionClear();
    return jex;
}

inline jhthrowable jexception()
{
    return jexception(jnienv());
}

// Raise native exception constructed from Java one
// By default just abort the application
// If you want other behaviour, just redefine it in your code
void rethrow(JNIEnv *env, jthrowable ex);

inline void rethrow(jthrowable ex)
{
    rethrow(jnienv(), ex);
}

inline void rethrow(JNIEnv *env, jhthrowable const &ex)
{
    rethrow(env, ex.get());
}

inline void rethrow(jhthrowable const &ex)
{
    rethrow(jnienv(), ex);
}


} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_EXCEPTIONS_HPP_5302DB654A1C4F998698D3AE018E46A3 */
