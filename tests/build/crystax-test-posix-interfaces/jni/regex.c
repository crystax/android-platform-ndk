/*
 * Copyright (c) 2011-2014 CrystaX .NET.
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
 * THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX .NET.
 */

#if __ANDROID__
#include <android/api-level.h>
#endif

#if !__ANDROID__ || __ANDROID_API__ >= 8

#include <regex.h>

#include "gen/regex.inc"

#include "helper.h"

#define CHECK(type) type JOIN(regex_check_type_, __LINE__)

CHECK(regex_t);
CHECK(size_t);
CHECK(regoff_t);
CHECK(regmatch_t);

void regex_check_regex_t_fields(regex_t *s)
{
    s->re_nsub = (size_t)0;
}

void regex_check_regmatch_t_fields(regmatch_t *s)
{
    s->rm_so = (regoff_t)0;
    s->rm_eo = (regoff_t)0;
}

void regex_check_functions()
{
    (void)regcomp((regex_t*)0, (const char *)0, 0);
    (void)regerror(0, (const regex_t*)0, (char*)0, (size_t)0);
    (void)regexec((const regex_t*)0, (const char*)0, (size_t)0, (regmatch_t*)0, 0);
    (void)regfree((regex_t*)0);
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 8 */
