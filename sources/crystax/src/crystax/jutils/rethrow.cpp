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

#include <crystax.h>
#include <crystax/jutils.hpp>
#include <crystax/log.h>
#include <crystax/memory.hpp>

namespace crystax
{
namespace jni
{

void rethrow(JNIEnv *env, jthrowable ex)
{
    if (!ex) return;

    jhclass cls(env->GetObjectClass(ex));
    if (!cls) CRYSTAX_PANIC("Can't get object class for Java throwable object");

    jmethodID mid = env->GetMethodID(cls.get(), "getMessage", "()Ljava/lang/String;");
    if (!mid) CRYSTAX_PANIC("Can't get 'getMessage' method id for Java throwable object");

    jstring msg = (jstring)env->CallObjectMethod(ex, mid);
    if (!msg) CRYSTAX_PANIC("Can't call 'getMessage' for Java throwable object");

    scope_c_ptr_t<const char> s(jcast<const char *>(msg));
    CRYSTAX_ERR("Java exception: %s", s.get());

    env->ExceptionDescribe();
    env->ExceptionClear();

    ::abort();
}

} // namespace jni
} // namespace crystax
