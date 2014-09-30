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

#if !defined(__LP64__) || !__LP64__

/* 32-bit Android libc don't provide pthread_mutex_timedlock so we implement it on our own
 * There is no need to do that for 64-bit target since pthread_mutex_timedlock is implemented there
 */

/* return difference in milliseconds */
static long long diff(const struct timespec *s, const struct timespec *e)
{
    long long start = ((long long) s->tv_sec * 1000LL) + ((long long) s->tv_nsec / 1000000);
    long long end   = ((long long) e->tv_sec * 1000LL) + ((long long) e->tv_nsec / 1000000);

    return end - start;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime)
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
    rc = pthread_mutex_lock_timeout_np(mutex, (unsigned) msecs);
    if (rc == EBUSY)
        rc = ETIMEDOUT;

    return rc;
}

#endif /* !defined(__LP64__) || !__LP64__ */
