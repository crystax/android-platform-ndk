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

#include <netinet/in.h>

#include "gen/netinet_in.inc"

#include "helper.h"

#define CHECK(type) type JOIN(netinet_in_check_type_, __LINE__)

CHECK(in_port_t);
CHECK(in_addr_t);
CHECK(sa_family_t);
CHECK(uint8_t);
CHECK(uint32_t);
CHECK(struct in_addr);
CHECK(struct sockaddr_in);

#if _POSIX_IPV6 > 0
CHECK(struct in6_addr);
CHECK(struct sockaddr_in6);
CHECK(struct ipv6_mreq);

/* Check external variables */
const struct in6_addr *p1 = &in6addr_any;
const struct in6_addr *p2 = &in6addr_loopback;
#endif /* _POSIX_IPV6 > 0 */

#undef CHECK
#define CHECK(name) void netinet_in_check_ ## name ## _fields(struct name *s)

CHECK(in_addr)
{
    s->s_addr = (in_addr_t)0;
}

CHECK(sockaddr_in)
{
    s->sin_family = (sa_family_t)0;
    s->sin_port = (in_port_t)0;
    netinet_in_check_in_addr_fields(&s->sin_addr);
}

#if _POSIX_IPV6 > 0

CHECK(in6_addr)
{
    s->s6_addr[0] = (uint8_t)0;
    CTASSERT(sizeof(s->s6_addr) == 16);
}

CHECK(sockaddr_in6)
{
    s->sin6_family = (sa_family_t)0;
    s->sin6_port = (in_port_t)0;
    s->sin6_flowinfo = (uint32_t)0;
    netinet_in_check_in6_addr_fields(&s->sin6_addr);
    s->sin6_scope_id = (uint32_t)0;
}

CHECK(ipv6_mreq)
{
    netinet_in_check_in6_addr_fields(&s->ipv6mr_multiaddr);
    s->ipv6mr_interface = 0;
}

#endif /* _POSIX_IPV6 > 0 */
