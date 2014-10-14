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

#ifndef __CRYSTAX_INCLUDE_SYS_LIMITS_H_AB5D38F5B8194E07AAFB4280C9468A53
#define __CRYSTAX_INCLUDE_SYS_LIMITS_H_AB5D38F5B8194E07AAFB4280C9468A53

#include <crystax/id.h>

#include <linux/limits.h>

#include <android/api-level.h>

#ifndef PAGE_SIZE
#if __ANDROID_API__ < 21
#include <asm/page.h>
#else
#define PAGE_SIZE 4096
#endif
#endif

#ifndef PAGESIZE
#define PAGESIZE PAGE_SIZE
#endif

#ifndef PTHREAD_STACK_MIN
#if defined(__LP64__)
#define PTHREAD_STACK_MIN (4 * PAGE_SIZE)
#else
#define PTHREAD_STACK_MIN (2 * PAGE_SIZE)
#endif
#endif

#define  CHILD_MAX   999
#define  OPEN_MAX    256

#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4
#define PTHREAD_DESTRUCTOR_ITERATIONS _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#define _POSIX_THREAD_KEYS_MAX 128
#define PTHREAD_KEYS_MAX _POSIX_THREAD_KEYS_MAX
#define _POSIX_THREAD_THREADS_MAX 64
#define PTHREAD_THREADS_MAX /* bionic has no specific limit */

#define  _POSIX_VERSION             200809L   /* Posix C language bindings version */
#define  _POSIX2_VERSION            (-1)      /* we don't support Posix command-line tools */
#define  _POSIX2_C_VERSION          _POSIX_VERSION
#define  _XOPEN_VERSION             700       /* by Posix definition */
#define  _XOPEN_XCU_VERSION         (-1)      /* we don't support command-line utilities */

#define  _POSIX_SHELL               (-1)

#if _POSIX_VERSION > 0
#define  _XOPEN_XPG2                1
#define  _XOPEN_XPG3                1
#define  _XOPEN_XPG4                1
#define  _XOPEN_UNIX                1
#endif

#define  _XOPEN_ENH_I18N          (-1)  /* we don't support internationalization in the C library */
#define  _XOPEN_CRYPT             (-1)  /* don't support X/Open Encryption */
#define  _XOPEN_LEGACY            (-1)  /* don't claim we support these, we have some of them but not all */
#define  _XOPEN_REALTIME          (-1)  /* we don't support all these functions */
#define  _XOPEN_REALTIME_THREADS  (-1)  /* same here */

#define  _POSIX_REALTIME_SIGNALS     (-1)
#define  _POSIX_PRIORITY_SCHEDULING  200809L
#define  _POSIX_TIMEOUTS             200809L
#define  _POSIX_TIMERS               200809L
#undef   _POSIX_ASYNCHRONOUS_IO         /* aio_ functions are not supported */
#define  _POSIX_SYNCHRONIZED_IO      200809L
#define  _POSIX_FSYNC                200809L
#define  _POSIX_MAPPED_FILES         200809L
#define  _POSIX_MEMORY_PROTECTION    200809L

#if __ANDROID_API__ < 9
#define  _POSIX_READER_WRITER_LOCKS   (-1)
#else
#define  _POSIX_READER_WRITER_LOCKS   200809L
#endif

#define  _POSIX_REGEXP                200809L

#define  _POSIX_THREADS               200809L

#define  _POSIX_THREAD_ATTR_STACKADDR 200809L
#define  _POSIX_THREAD_STACKADDR _POSIX_THREAD_ATTR_STACKADDR

#define  _POSIX_THREAD_ATTR_STACKSIZE 200809L
#define  _POSIX_THREAD_STACKSIZE _POSIX_THREAD_ATTR_STACKSIZE

#define  _POSIX_THREAD_PRIO_INHERIT   200809L
#define  _POSIX_THREAD_PRIO_PROTECT   200809L

#define  _POSIX_MONOTONIC_CLOCK       200809L
#define  _POSIX_SAVED_IDS             200809L

#undef   _POSIX_PROCESS_SHARED

/* Some thread-safe functions not implemented yet:
 * getgrgid_r
 * getgrnam_r
 * getpwnam_r
 * getpwuid_r
 */
#undef   _POSIX_THREAD_SAFE_FUNCTIONS

#define  _POSIX_CHOWN_RESTRICTED    1    /* yes, chown requires appropriate privileges */
#define  _POSIX_NO_TRUNC            1    /* very long pathnames generate an error */
#define  _POSIX_JOB_CONTROL         1    /* job control is a Linux feature */

#endif /* __CRYSTAX_INCLUDE_SYS_LIMITS_H_AB5D38F5B8194E07AAFB4280C9468A53 */
