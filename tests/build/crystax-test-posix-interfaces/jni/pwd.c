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

#include <pwd.h>
#include "helper.h"

#define CHECK(type) type JOIN(pwd_check_type_, __LINE__)

CHECK(struct passwd);
CHECK(gid_t);
CHECK(uid_t);
CHECK(size_t);

void pwd_check_passwd_fields(struct passwd *s)
{
    s->pw_name  = (char*)0;
    s->pw_uid   = (uid_t)0;
    s->pw_gid   = (gid_t)0;
    s->pw_dir   = (char*)0;
    s->pw_shell = (char*)0;
    s->pw_gecos = (char*)0;
}

void pwd_check_functions()
{
#if __XSI_VISIBLE
    (void)endpwent();
    (void)getpwent();
#endif
    (void)getpwnam((const char *)0);
    (void)getpwnam_r((const char *)0, (struct passwd *)0, (char *)0, (size_t)0, (struct passwd **)0);
    (void)getpwuid((uid_t)0);
    (void)getpwuid_r((uid_t)0, (struct passwd *)0, (char *)0, (size_t)0, (struct passwd **)0);
#if __XSI_VISIBLE
    (void)setpwent();
#endif
}
