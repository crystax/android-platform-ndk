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

#ifndef _CRYSTAX_INTERNAL_H_800619B1E3CF4547AD9EEABF49101679
#define _CRYSTAX_INTERNAL_H_800619B1E3CF4547AD9EEABF49101679

#include <stdint.h>
#include <xlocale.h>
#include <machine/_align.h>

/* Size of long double should be either 64- or 128-bit */
#if __LDBL_MANT_DIG__ != 53 && __LDBL_MANT_DIG__ != 113
#error "Wrong size of long double"
#endif

#define ALIGNBYTES _ALIGNBYTES
#define ALIGN(p) _ALIGN(p)

#define FLOCKFILE(fp)   if (__isthreaded) flockfile(fp)
#define FUNLOCKFILE(fp) if (__isthreaded) funlockfile(fp)

extern void __crystax_stdio_thread_lock();
extern void __crystax_stdio_thread_unlock();
#define STDIO_THREAD_LOCK()   __crystax_stdio_thread_lock()
#define STDIO_THREAD_UNLOCK() __crystax_stdio_thread_unlock()

/*
 * Function to clean up streams, called from abort() and exit().
 */
extern void (*__cleanup)(void) __attribute__((__visibility__("hidden")));

#ifndef NBBY
#define NBBY 8
#endif

#define _pthread_mutex_lock(m)     pthread_mutex_lock(m)
#define  _pthread_mutex_trylock(m) pthread_mutex_trylock(m)
#define _pthread_mutex_unlock(m)   pthread_mutex_unlock(m)
#define _pthread_mutex_destroy(m)  pthread_mutex_destroy(m)
#define _pthread_self()            pthread_self()

#define _once(o, f) pthread_once(o, f)

#define _fcntl fcntl

#define _close close
#define _fstat fstat
#define _getprogname getprogname
#define _open open
#define _openat openat
#define _read read
#define _write write

#define _dup2(fd, fd2) dup2(fd, fd2)

#define _sigprocmask sigprocmask

#endif /* _CRYSTAX_INTERNAL_H_800619B1E3CF4547AD9EEABF49101679 */
