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

#if !__ANDROID__ || __ANDROID_API__ >= 9

#include <sys/wait.h>

#include "gen/sys_wait.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_wait_check_type_, __LINE__)

CHECK(idtype_t);
CHECK(id_t);
CHECK(pid_t);
CHECK(siginfo_t);

#undef CHECK
#define CHECK(v) sys_wait_idtype_t_helper(v)

void sys_wait_idtype_t_helper(idtype_t t)
{
    (void)t;
}

void sys_wait_check_idtype_t_values()
{
    CHECK(P_ALL);
    CHECK(P_PGID);
    CHECK(P_PID);
}

void sys_wait_check_functions()
{
    (void)wait((int*)1234);
    (void)waitid((idtype_t)0, (id_t)0, (siginfo_t*)1234, 0);
    (void)waitpid((pid_t)0, (int*)1234, 0);
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 9 */
