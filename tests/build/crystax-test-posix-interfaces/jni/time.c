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

#include <time.h>
#if __APPLE__
#include <xlocale.h>
#endif
#if __APPLE__ || __gnu_linux__
#include <signal.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_TIME_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in time.h
#endif
#endif

#include "gen/time.inc"

#include "helper.h"

#define CHECK(type) type JOIN(time_check_type_, __LINE__)

CHECK(clock_t);
CHECK(size_t);
CHECK(time_t);
#if _POSIX_CLOCK_SELECTION > 0
CHECK(clockid_t);
#endif
#if _POSIX_TIMERS > 0
CHECK(timer_t);
#endif
CHECK(locale_t);
#if _POSIX_CPUTIME > 0
CHECK(pid_t);
#endif
CHECK(struct sigevent);
CHECK(struct tm);
CHECK(struct timespec);
#if _POSIX_TIMERS > 0
CHECK(struct itimerspec);
#endif

#undef CHECK
#define CHECK(name) void JOIN(time_check_fields_, __LINE__)(name *s)

CHECK(struct tm)
{
    s->tm_sec   = 0;
    s->tm_min   = 0;
    s->tm_hour  = 0;
    s->tm_mday  = 0;
    s->tm_mon   = 0;
    s->tm_year  = 0;
    s->tm_wday  = 0;
    s->tm_yday  = 0;
    s->tm_isdst = 0;
}

CHECK(struct timespec)
{
    s->tv_sec  = (time_t)0;
    s->tv_nsec = (long)0;
}

#if _POSIX_TIMERS > 0
CHECK(struct itimerspec)
{
    struct timespec *p1, *p2;
    p1 = &s->it_interval;
    p2 = &s->it_value;
    (void)p1;
    (void)p2;
}
#endif /* _POSIX_TIMERS > 0 */

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */

struct tm *getdate(const char *s)
{
    (void)s;
    return (struct tm *)0;
}

#endif /* __ANDROID__ */

void time_check_functions(struct tm *ptm, const struct tm *cptm,
        struct timespec *pts, const struct timespec *cpts, locale_t l
#if _POSIX_TIMERS > 0
        , clockid_t ci
        , timer_t ti
#endif
        )
{
    (void)asctime(cptm);
    (void)asctime_r(cptm, (char*)1234);
    (void)clock();
#if _POSIX_CPUTIME > 0
    (void)clock_getcpuclockid((pid_t)0, (clockid_t*)1234);
#endif
#if _POSIX_TIMERS > 0
    (void)clock_getres(ci, pts);
    (void)clock_gettime(ci, pts);
#endif
#if _POSIX_CLOCK_SELECTION > 0
    (void)clock_nanosleep(ci, 0, cpts, pts);
#endif
#if _POSIX_TIMERS > 0
    (void)clock_settime(ci, cpts);
#endif
    (void)ctime((const time_t *)1234);
    (void)ctime_r((const time_t *)1234, (char *)1234);
    (void)difftime((time_t)0, (time_t)0);
    (void)getdate((const char *)1234);
    (void)gmtime((const time_t *)1234);
    (void)gmtime_r((const time_t *)1234, ptm);
    (void)localtime((const time_t *)1234);
    (void)localtime_r((const time_t *)1234, ptm);
    (void)mktime(ptm);
    (void)nanosleep(cpts, pts);
    (void)strftime((char*)1234, (size_t)0, "%a", cptm);
    (void)strftime_l((char*)1234, (size_t)0, "%a", cptm, l);
    (void)strptime((const char *)1234, (const char *)1234, ptm);
#if __APPLE__ || __ANDROID__
    (void)strptime_l((const char *)1234, (const char *)1234, ptm, l);
#endif
    (void)time((time_t*)1234);
#if _POSIX_TIMERS > 0
    (void)timer_create(ci, (struct sigevent *)1234, &ti);
    (void)timer_delete(ti);
    (void)timer_getoverrun(ti);
    (void)timer_gettime(ti, (struct itimerspec *)1234);
    (void)timer_settime(ti, 0, (const struct itimerspec *)1234, (struct itimerspec *)1234);
#endif
    (void)tzset();
}

void time_check_variables(int *d, long *t, char **z)
{
    *d = daylight;
    *t = timezone;
    *z = tzname[0];
}
