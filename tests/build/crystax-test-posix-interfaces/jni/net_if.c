/*
 * Copyright (c) 2011-2015 CrystaX.
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

#if __APPLE__
#include <sys/socket.h>
#endif

#include <net/if.h>

#ifndef IF_NAMESIZE
#error  'IF_NAMESIZE' not defined
#endif

#include "helper.h"

#define CHECK(type) type JOIN(net_if_check_type_, __LINE__)

#if __ANDROID__
/* WARNING!!! These functions are not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

struct if_nameindex
{
    unsigned if_index;
    char *   if_name;
};

void if_freenameindex(struct if_nameindex *s)
{
    (void)s;
}

struct if_nameindex *if_nameindex()
{
    return (struct if_nameindex *)0;
}
#endif

CHECK(struct if_nameindex);

void net_if_check_if_nameindex_fields(struct if_nameindex *s)
{
    s->if_index = 0;
    s->if_name  = (char*)0;
}

void net_if_check_functions(struct if_nameindex *s)
{
    (void)if_freenameindex(s);
    (void)if_indextoname((unsigned)0, (char*)0);
    (void)if_nameindex();
    (void)if_nametoindex((const char *)0);
}
