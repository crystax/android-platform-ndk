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

#ifndef __CRYSTAX_INCLUDE_FCNTL_H_C1EB0D2834804E14818592AF5D8C0816
#define __CRYSTAX_INCLUDE_FCNTL_H_C1EB0D2834804E14818592AF5D8C0816

#include <crystax/id.h>
#include <crystax/google/fcntl.h>

#define __CRYSTAX_FCNTL_BASE 0x80000000

#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC  ((int)(__CRYSTAX_FCNTL_BASE | 0x0001))
#endif

#define F_DUP2FD         ((int)(__CRYSTAX_FCNTL_BASE | 0x0002))
#define F_DUP2FD_CLOEXEC ((int)(__CRYSTAX_FCNTL_BASE | 0x0003))

#ifndef F_GETPIPE_SZ
#define F_GETPIPE_SZ     ((int)(__CRYSTAX_FCNTL_BASE | 0x0004))
#endif

#ifndef F_SETPIPE_SZ
#define F_SETPIPE_SZ     ((int)(__CRYSTAX_FCNTL_BASE | 0x0005))
#endif

#define POSIX_FADV_NORMAL     0
#define POSIX_FADV_RANDOM     1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED   3
#define POSIX_FADV_DONTNEED   4
#define POSIX_FADV_NOREUSE    5

__BEGIN_DECLS

extern int fallocate64(int, int, off64_t, off64_t);
extern int fallocate(int, int, off_t, off_t);

extern int posix_fallocate64(int, off64_t, off64_t);
extern int posix_fallocate(int, off_t, off_t);

extern int posix_fadvise64(int, off64_t, off64_t, int);
extern int posix_fadvise(int, off_t, off_t, int);

__END_DECLS

#endif /* __CRYSTAX_INCLUDE_FCNTL_H_C1EB0D2834804E14818592AF5D8C0816 */
