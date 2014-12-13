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

#include <crystax/atomic.h>
#include <crystax/bionic.h>

static int initialized = 0;
static int (*bionic_pthread_mutex_timedlock)(pthread_mutex_t *, const struct timespec *) = NULL;
static int (*bionic_pthread_mutex_lock_timeout_np)(pthread_mutex_t *, unsigned int ) = NULL;

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

    /* See http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_mutex_timedlock.html:
     * Under no circumstance shall the function fail with a timeout if the mutex can be locked immediately.
     * The validity of the abs_timeout parameter need not be checked if the mutex can be locked immediately.
     */
    rc = pthread_mutex_trylock(mutex);
    if (rc == 0)
        return 0;

    if (!abstime || (abstime->tv_nsec < 0) || (abstime->tv_nsec >= 1000000000))
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
    rc = bionic_pthread_mutex_lock_timeout_np(mutex, (unsigned) msecs);
    if (rc == EBUSY)
        rc = ETIMEDOUT;

    return rc;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
{
    if (!mutex)
        return EINVAL;

    if (__crystax_atomic_fetch(&initialized) == 0)
    {
        bionic_pthread_mutex_timedlock = __crystax_bionic_symbol(__CRYSTAX_BIONIC_SYMBOL_PTHREAD_MUTEX_TIMEDLOCK, 1);
        bionic_pthread_mutex_lock_timeout_np = __crystax_bionic_symbol(__CRYSTAX_BIONIC_SYMBOL_PTHREAD_MUTEX_LOCK_TIMEOUT_NP, 1);

        __crystax_atomic_swap(&initialized, 1);
    }

    if (bionic_pthread_mutex_timedlock)
        return bionic_pthread_mutex_timedlock(mutex, abstime);
    else if (bionic_pthread_mutex_lock_timeout_np)
        return crystax_pthread_mutex_timedlock(mutex, abstime);
    else
        return EFAULT;
}
