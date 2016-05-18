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

extern int __isthreaded;

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

#define _pthread_getspecific(x)    pthread_getspecific(x)
#define _pthread_key_create(x, y)  pthread_key_create(x, y)
#define _pthread_mutex_destroy(x)  pthread_mutex_destroy(x)
#define _pthread_mutex_lock(x)     pthread_mutex_lock(x)
#define _pthread_mutex_trylock(x)  pthread_mutex_trylock(x)
#define _pthread_mutex_unlock(x)   pthread_mutex_unlock(x)
#define _pthread_once(x, y)        pthread_once(x, y)
#define _pthread_rwlock_rdlock(x)  pthread_rwlock_rdlock(x)
#define _pthread_rwlock_unlock(x)  pthread_rwlock_unlock(x)
#define _pthread_rwlock_wrlock(x)  pthread_rwlock_wrlock(x)
#define _pthread_self()            pthread_self()
#define _pthread_setspecific(x, y) pthread_setspecific(x, y)

#define _dup2(fd, fd2) dup2(fd, fd2)
#define _once(o, f) pthread_once(o, f)

#define _close close
#define _dirfd dirfd
#define _fcntl fcntl
#define _fstat fstat
#define _getprogname getprogname
#define _open open
#define _openat openat
#define _read read
#define _sigaction sigaction
#define _sigprocmask sigprocmask
#define _wait4 wait4
#define _write write
#define _writev writev

#endif /* _CRYSTAX_INTERNAL_H_800619B1E3CF4547AD9EEABF49101679 */
