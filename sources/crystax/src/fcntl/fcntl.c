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
#include <crystax/bionic.h>

static int crystax_fcntl(int (*bionic_fcntl)(int, int, ...), int fd, int command, va_list args)
{
    int rc;
    int fd2;

    switch (command)
    {
        case F_DUPFD_CLOEXEC:
            rc = bionic_fcntl(fd, F_DUPFD);
            if (rc < 0) return rc;
            fd2 = rc;
            rc = bionic_fcntl(fd, F_SETFD, FD_CLOEXEC);
            if (rc < 0)
                close(fd2);
            return rc;
        case F_DUP2FD:
        case F_DUP2FD_CLOEXEC:
            fd2 = va_arg(args, int);
            rc = dup2(fd, fd2);
            if (rc < 0) return rc;
            if (command == F_DUP2FD_CLOEXEC)
            {
                rc = bionic_fcntl(fd, F_SETFD, FD_CLOEXEC);
                if (rc < 0)
                    close(fd2);
            }
            return rc;
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

    int (*bionic_fcntl)(int, int, ...);

    bionic_fcntl = __crystax_bionic_symbol(__CRYSTAX_BIONIC_SYMBOL_FCNTL, 0);

    if (command & __CRYSTAX_FCNTL_BASE)
    {
        va_start(args, command);
        rc = crystax_fcntl(bionic_fcntl, fd, command, args);
        va_end(args);
        return rc;
    }

    switch (command)
    {
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_GETFD:
        case F_GETFL:
        case F_GETLEASE:
        case F_GETOWN:
        case F_GETPIPE_SZ:
        case F_GETSIG:
            return bionic_fcntl(fd, command);
        case F_NOTIFY:
        case F_SETFD:
        case F_SETFL:
        case F_SETLEASE:
        case F_SETOWN:
        case F_SETPIPE_SZ:
        case F_SETSIG:
            va_start(args, command);
            n = va_arg(args, int);
            rc = bionic_fcntl(fd, command, n);
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
            rc = bionic_fcntl(fd, command, flck);
            va_end(args);
            return rc;
        default:
            DBG("Unknown fcntl command: %d", command);
            errno = EINVAL;
            return -1;
    }
}
