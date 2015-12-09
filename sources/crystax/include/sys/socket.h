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

#ifndef __CRYSTAX_INCLUDE_SYS_SOCKET_H_4BFF8F85ACF54B99BE2248B9D829D71F
#define __CRYSTAX_INCLUDE_SYS_SOCKET_H_4BFF8F85ACF54B99BE2248B9D829D71F

#include <crystax/id.h>

#include <android/api-level.h>

#if __ANDROID_API__ >= 21
#include <crystax/google/sys/socket.h>
#else /* __ANDROID_API__ < 21 */

#include <sys/cdefs.h>
#include <sys/types.h>
#include <linux/socket.h>

__BEGIN_DECLS

#define SOCK_STREAM      1
#define SOCK_DGRAM       2
#define SOCK_RAW         3
#define SOCK_RDM         4
#define SOCK_SEQPACKET   5
#define SOCK_PACKET      10

#ifdef __i386__
# define __socketcall __attribute__((__cdecl__))
#else
# define __socketcall
#endif

#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2

__socketcall int socket(int, int, int);
__socketcall int bind(int, const struct sockaddr *, int);
__socketcall int connect(int, const struct sockaddr *, socklen_t);
__socketcall int listen(int, int);
__socketcall int accept(int, struct sockaddr *, socklen_t *);
__socketcall int getsockname(int, struct sockaddr *, socklen_t *);
__socketcall int getpeername(int, struct sockaddr *, socklen_t *);
__socketcall int socketpair(int, int, int, int *);
__socketcall int shutdown(int, int);
__socketcall int setsockopt(int, int, int, const void *, socklen_t);
__socketcall int getsockopt(int, int, int, void *, socklen_t *);
__socketcall int sendmsg(int, const struct msghdr *, unsigned int);
__socketcall int recvmsg(int, struct msghdr *, unsigned int);

ssize_t  send(int, const void *, size_t, unsigned int);
ssize_t  recv(int, void *, size_t, unsigned int);

__socketcall ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
__socketcall ssize_t recvfrom(int, void *, size_t, unsigned int, const struct sockaddr *, socklen_t *);

#undef __socketcall

__END_DECLS

#endif /* __ANDROID_API__ < 21 */

#endif /* __CRYSTAX_INCLUDE_SYS_SOCKET_H_4BFF8F85ACF54B99BE2248B9D829D71F */
