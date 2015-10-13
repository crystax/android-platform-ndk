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

#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <crystax/log.h>

extern int __fcntl64(int fd, int command, ...);

static int dupfd_cloexec(int fd, int fd2)
{
    int rc = __fcntl64(fd, F_DUPFD, fd2);
    if (rc < 0) return -1;

    if (__fcntl64(rc, F_SETFD, FD_CLOEXEC) < 0)
    {
        int serrno = errno;
        close(rc);
        errno = serrno;
        return -1;
    }

    return rc;
}

static int dup2fd_cloexec(int fd, int fd2)
{
    int rc = dup2(fd, fd2);
    if (rc < 0) return -1;

    if (__fcntl64(rc, F_SETFD, FD_CLOEXEC) < 0)
    {
        int serrno = errno;
        close(rc);
        errno = serrno;
        return -1;
    }

    return rc;
}

static int crystax_fcntl(int fd, int command, va_list args)
{
    switch (command)
    {
        case F_DUPFD_CLOEXEC:
            return dupfd_cloexec(fd, va_arg(args, int));
        case F_DUP2FD:
            return dup2(fd, va_arg(args, int));
        case F_DUP2FD_CLOEXEC:
            return dup2fd_cloexec(fd, va_arg(args, int));
        default:
            DBG("Unknown crystax_fcntl command: %d", command);
            errno = EINVAL;
            return -1;
    }
}

int fcntl(int fd, int command, ...)
{
    va_list args;
    int rc;
    int n;
    struct flock *flck;

    switch (command)
    {
        case F_DUPFD_CLOEXEC:
        case F_DUP2FD:
        case F_DUP2FD_CLOEXEC:
            va_start(args, command);
            rc = crystax_fcntl(fd, command, args);
            va_end(args);
            return rc;
        case F_GETFD:
        case F_GETFL:
        case F_GETLEASE:
        case F_GETOWN:
        case F_GETPIPE_SZ:
        case F_GETSIG:
            return __fcntl64(fd, command);
        case F_NOTIFY:
        case F_DUPFD:
        case F_SETFD:
        case F_SETFL:
        case F_SETLEASE:
        case F_SETOWN:
        case F_SETPIPE_SZ:
        case F_SETSIG:
            va_start(args, command);
            n = va_arg(args, int);
            rc = __fcntl64(fd, command, n);
            va_end(args);
            return rc;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
#if !__LP64__
        case F_GETLK64:
        case F_SETLK64:
        case F_SETLKW64:
#endif
            va_start(args, command);
            flck = va_arg(args, struct flock *);
            rc = __fcntl64(fd, command, flck);
            va_end(args);
            return rc;
        default:
            DBG("Unknown fcntl command: %d", command);
            errno = EINVAL;
            return -1;
    }
}
