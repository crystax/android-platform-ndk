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

#include <sched.h>

#include "gen/sched.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sched_check_type_, __LINE__)

#if _POSIX_PRIORITY_SCHEDULING > 0
CHECK(pid_t);
#endif

#if _POSIX_SPORADIC_SERVER > 0 || _POSIX_THREAD_SPORADIC_SERVER > 0
CHECK(time_t);
CHECK(struct timespec);
#endif

CHECK(struct sched_param);

void sched_check_sched_param_fields(struct sched_param *s)
{
    s->sched_priority = 0;
#if _POSIX_SPORADIC_SERVER > 0 || _POSIX_THREAD_SPORADIC_SERVER > 0
    s->sched_ss_low_priority = 0;
    s->sched_ss_repl_period.tv_sec  = 0;
    s->sched_ss_repl_period.tv_nsec = 0;
    s->sched_ss_init_budget.tv_sec  = 0;
    s->sched_ss_init_budget.tv_nsec = 0;
    s->sched_ss_max_repl = 0;
#endif /* _POSIX_SPORADIC_SERVER > 0 && _POSIX_THREAD_SPORADIC_SERVER > 0 */
}

void sched_check_functions()
{
#if _POSIX_PRIORITY_SCHEDULING > 0 || _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)sched_get_priority_max(0);
    (void)sched_get_priority_min(0);
    (void)sched_rr_get_interval((pid_t)0, (struct timespec *)0);
#endif

#if _POSIX_PRIORITY_SCHEDULING > 0
    (void)sched_getparam((pid_t)0, (struct sched_param *)0);
    (void)sched_getscheduler((pid_t)0);
    (void)sched_setparam((pid_t)0, (const struct sched_param *)0);
    (void)sched_setscheduler((pid_t)0, 0, (const struct sched_param *)0);
#endif

    (void)sched_yield();
}
