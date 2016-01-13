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

#ifndef _CRYSTAX_JUTILS_CLASS_HPP_7C2D7E9AE34B45A19E57CA4D43E91F7D
#define _CRYSTAX_JUTILS_CLASS_HPP_7C2D7E9AE34B45A19E57CA4D43E91F7D

#include <crystax/id.h>

#ifndef __cplusplus
#error "This is C++ header file, you shouldn't include it to non-C++ sources"
#endif

#include <crystax/jutils/jni.hpp>
#include <crystax/jutils/jholder.hpp>
#include <crystax/jutils/exceptions.hpp>

namespace crystax
{
namespace jni
{

inline
jhclass find_class(JNIEnv *env, const char *clsname)
{
    jhclass ret((jclass)env->FindClass(clsname));
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return ret;
}

inline
jhclass find_class(const char *clsname)
{
    return find_class(jnienv(), clsname);
}

template <typename T, typename Refcounter>
jhclass get_class(JNIEnv *env, jholder<T, Refcounter> const &obj)
{
    jhclass ret(env->GetObjectClass(obj.get()));
    auto ex = jexception(env);
    if (ex) rethrow(ex);
    return ret;
}

template <typename T, typename Refcounter>
jhclass get_class(jholder<T, Refcounter> const &obj)
{
    return get_class(jnienv(), obj);
}

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_CLASS_HPP_7C2D7E9AE34B45A19E57CA4D43E91F7D */
