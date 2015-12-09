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

#include <sys/socket.h>

#include "gen/sys_socket.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_socket_check_type_, __LINE__)

CHECK(socklen_t);
CHECK(sa_family_t);
CHECK(struct sockaddr);
CHECK(struct sockaddr_storage);
CHECK(struct msghdr);
CHECK(struct iovec);
CHECK(struct cmsghdr);
CHECK(struct linger);
CHECK(size_t);
CHECK(ssize_t);

#undef CHECK
#define CHECK(name) void JOIN(sys_socket_check_fields_, __LINE__)(name *s)

CHECK(struct sockaddr)
{
    s->sa_family  = (sa_family_t)0;
    s->sa_data[0] = (char)0;
}

CHECK(struct sockaddr_storage)
{
    s->ss_family = (sa_family_t)0;
}

CHECK(struct msghdr)
{
    s->msg_name       = (void*)0;
    s->msg_namelen    = (socklen_t)0;
    s->msg_iov        = (struct iovec *)0;
    s->msg_iovlen     = 0;
    s->msg_control    = (void*)0;
    s->msg_controllen = (socklen_t)0;
    s->msg_flags      = 0;
}

CHECK(struct cmsghdr)
{
    s->cmsg_len   = (socklen_t)0;
    s->cmsg_level = 0;
    s->cmsg_type  = 0;
}

CHECK(struct linger)
{
    s->l_onoff  = 0;
    s->l_linger = 0;
}

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: Fix that */

int sockatmark(int s)
{
    (void)s;
    return -1;
}

#endif /* __ANDROID__ */

void sys_socket_check_functions(struct sockaddr *sa, socklen_t sl, socklen_t *psl)
{
    (void)accept(0, sa, psl);
    (void)bind(0, sa, sl);
    (void)connect(0, sa, sl);
    (void)getpeername(0, sa, psl);
    (void)getsockname(0, sa, psl);
    (void)getsockopt(0, 0, 0, (void*)1234, psl);
    (void)listen(0, 0);
    (void)recv(0, (void*)1234, (size_t)1234, 0);
    (void)recvfrom(0, (void*)1234, (size_t)1234, 0, sa, psl);
    (void)recvmsg(0, (struct msghdr *)1234, 0);
    (void)send(0, (const void *)1234, (size_t)1234, 0);
    (void)sendmsg(0, (const struct msghdr *)1234, 0);
    (void)sendto(0, (const void *)1234, (size_t)1234, 0, (const struct sockaddr *)1234, sl);
    (void)setsockopt(0, 0, 0, (const void *)1234, sl);
    (void)shutdown(0, 0);
    (void)sockatmark(0);
    (void)socket(0, 0, 0);
    (void)socketpair(0, 0, 0, (int*)1234);
}
