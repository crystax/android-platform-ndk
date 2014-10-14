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

#include <grp.h>

#include "helper.h"

#define CHECK(type) type JOIN(grp_check_type_, __LINE__)

CHECK(gid_t);
CHECK(size_t);
CHECK(struct group);

struct group grp_check_fields = {
    .gr_name = (char*)0,
    .gr_gid  = (gid_t)0,
    .gr_mem  = (char**)0,
};

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

void endgrent()
{}

struct group *getgrent()
{
    return (struct group *)0;
}

void setgrent()
{}

int getgrgid_r(gid_t gid, struct group *grp,
               char *buf, size_t buflen, struct group **result)
{
    (void)gid;
    (void)grp;
    (void)buf;
    (void)buflen;
    (void)result;
    return -1;
}

int getgrnam_r(const char *name, struct group *grp,
               char *buf, size_t buflen, struct group **result)
{
    (void)name;
    (void)grp;
    (void)buf;
    (void)buflen;
    (void)result;
    return -1;
}

#endif /* __ANDROID__ */

void grp_check_functions()
{
#if __XSI_VISIBLE
    (void)endgrent();
    (void)getgrent();
#endif
    (void)getgrgid((gid_t)0);
    (void)getgrgid_r((gid_t)0, (struct group *)0, (char*)0, (size_t)0, (struct group **)0);
    (void)getgrnam("");
    (void)getgrnam_r("", (struct group *)0, (char*)0, (size_t)0, (struct group **)0);
#if __XSI_VISIBLE
    (void)setgrent();
#endif
}
