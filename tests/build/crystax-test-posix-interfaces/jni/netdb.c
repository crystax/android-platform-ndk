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

#include <netdb.h>

#include "gen/netdb.inc"

#include "helper.h"

#define CHECK(type) type JOIN(netdb_check_type_, __LINE__)

CHECK(struct hostent);
CHECK(struct netent);
CHECK(uint32_t);
CHECK(struct protoent);
CHECK(struct servent);
CHECK(struct addrinfo);
CHECK(socklen_t);

void netdb_check_hostent_fields(struct hostent *s)
{
    s->h_name      = (char*)0;
    s->h_aliases   = (char**)0;
    s->h_addrtype  = 0;
    s->h_length    = 0;
    s->h_addr_list = (char**)0;
}

void netdb_check_netent_fields(struct netent *s)
{
    s->n_name     = (char*)0;
    s->n_aliases  = (char**)0;
    s->n_addrtype = 0;
    s->n_net      = (uint32_t)0;
}

void netdb_check_protoent_fields(struct protoent *s)
{
    s->p_name    = (char*)0;
    s->p_aliases = (char**)0;
    s->p_proto   = 0;
}

void netdb_check_servent_fields(struct servent *s)
{
    s->s_name    = (char*)0;
    s->s_aliases = (char**)0;
    s->s_port    = 0;
    s->s_proto   = (char*)0;
}

void netdb_check_addrinfo_fields(struct addrinfo *s)
{
    s->ai_flags     = 0;
    s->ai_family    = 0;
    s->ai_socktype  = 0;
    s->ai_protocol  = 0;
    s->ai_addrlen   = (socklen_t)0;
    s->ai_addr      = (struct sockaddr *)0;
    s->ai_canonname = (char*)0;
    s->ai_next      = (struct addrinfo *)0;
}

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

void endhostent()
{}

void endnetent()
{}

void endprotoent()
{}

struct netent *getnetent()
{
    return (struct netent *)0;
}

struct protoent *getprotoent()
{
    return (struct protoent *)0;
}

void sethostent(int e)
{
    (void)e;
}

void setnetent(int e)
{
    (void)e;
}

void setprotoent(int e)
{
    (void)e;
}

#endif /* __ANDROID__ */

void netdb_check_functions()
{
    (void)endhostent();
    (void)endnetent();
    (void)endprotoent();
    (void)endservent();
    (void)freeaddrinfo((struct addrinfo *)0);
    (void)gai_strerror(0);
    (void)getaddrinfo((const char *)0, (const char *)0, (const struct addrinfo *)0, (struct addrinfo **)0);
    (void)gethostent();
    (void)getnameinfo((const struct sockaddr *)0, (socklen_t)0, (char*)0, (socklen_t)0, (char*)0, (socklen_t)0, 0);
    (void)getnetbyaddr((uint32_t)0, 0);
    (void)getnetbyname((const char *)0);
    (void)getnetent();
    (void)getprotobyname((const char *)0);
    (void)getprotobynumber(0);
    (void)getprotoent();
    (void)getservbyname((const char *)0, (const char *)0);
    (void)getservbyport(0, (const char *)0);
    (void)getservent();
    (void)sethostent(0);
    (void)setnetent(0);
    (void)setprotoent(0);
    (void)setservent(0);
}
