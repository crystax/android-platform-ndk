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

#ifndef _CRYSTAX_JUTILS_JCAST_HPP_05F3A127641F4DE98542AF1149D0B284
#define _CRYSTAX_JUTILS_JCAST_HPP_05F3A127641F4DE98542AF1149D0B284

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

template <typename T, typename U>
struct jcaster;

template <>
struct jcaster<const char *, jstring>
{
    const char *operator()(jstring v);
};

template <>
struct jcaster<jhstring, const char *>
{
    jhstring operator()(const char *s);
};

template <>
struct jcaster<const char *, jhstring>
{
    const char *operator()(jhstring const &v)
    {
        return jcaster<const char *, jstring>()(v.get());
    }
};

template <typename T, typename U>
T jcast(U const &v)
{
    return jcaster<T, U>()(v);
}

} // namespace jni
} // namespace crystax

#endif /* _CRYSTAX_JUTILS_JCAST_HPP_05F3A127641F4DE98542AF1149D0B284 */
