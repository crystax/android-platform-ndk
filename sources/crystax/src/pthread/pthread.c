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

#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <dlfcn.h>
#include <stdlib.h>

static int initialized = 0;
static int (*pthread_mutex_timedlock_func)(pthread_mutex_t *, const struct timespec *) = 0;
static int (*pthread_mutex_lock_timeout_np_func)(pthread_mutex_t *, unsigned int ) = 0;

static int crystax_atomic_fetch(volatile int *ptr)
{
    return __sync_add_and_fetch(ptr, 0);
}

static int crystax_atomic_swap(int v, volatile int *ptr)
{
    int prev;
    do
    {
        prev = *ptr;
    } while (__sync_val_compare_and_swap(ptr, prev, v) != prev);
    return prev;
}

/* return difference in milliseconds */
static long long diff(const struct timespec *s, const struct timespec *e)
{
    long long start = ((long long) s->tv_sec * 1000LL) + ((long long) s->tv_nsec / 1000000);
    long long end   = ((long long) e->tv_sec * 1000LL) + ((long long) e->tv_nsec / 1000000);

    return end - start;
}

static int crystax_pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    int rc = 0;
    long long msecs = 0;
    struct timespec curtime;

    if (abstime == NULL)
        return EINVAL;

    if ((abstime->tv_nsec < 0) || (abstime->tv_nsec >= 1000000000))
        return EINVAL;

    /* CLOCK_REALTIME is used here because it's required by IEEE Std
     * 1003.1, 2004 Edition */
    if (clock_gettime(CLOCK_REALTIME, &curtime) != 0)
        return errno;

    msecs = diff(&curtime, abstime);
    if (msecs <= 0)
        return ETIMEDOUT;
    if (msecs > UINT_MAX)
        return EINVAL;

    /* pthread_mutex_lock_timeout_np returns EBUSY when timeout expires
     * but POSIX specifies ETIMEDOUT return value */
    rc = pthread_mutex_lock_timeout_np_func(mutex, (unsigned) msecs);
    if (rc == EBUSY)
        rc = ETIMEDOUT;

    return rc;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    if (crystax_atomic_fetch(&initialized) == 0)
    {
        void *pc;

        pc = dlopen("libc.so", RTLD_NOW);
        if (!pc) abort();

        pthread_mutex_timedlock_func = dlsym(pc, "pthread_mutex_timedlock");
        pthread_mutex_lock_timeout_np_func = dlsym(pc, "pthread_mutex_lock_timeout_np");

        crystax_atomic_swap(1, &initialized);
    }

    if (pthread_mutex_timedlock_func)
        return pthread_mutex_timedlock_func(mutex, abstime);
    else if (pthread_mutex_lock_timeout_np_func)
        return crystax_pthread_mutex_timedlock(mutex, abstime);
    else
        return EFAULT;
}
