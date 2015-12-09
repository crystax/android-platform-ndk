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

#include <signal.h>
#if __APPLE__
#include <time.h>
#endif

#include "gen/signal.inc"

#include "helper.h"

extern void *memset(void *s, int c, size_t n);

#if __ANDROID__

/* https://tracker.crystax.net/issues/1158 */

void psiginfo(const siginfo_t *pinfo, const char *message)
{
    (void)pinfo;
    (void)message;
}

void psignal(int signum, const char *message)
{
    (void)signum;
    (void)message;
}

#endif /* !__ANDROID__ */

#define CHECK(type) type JOIN(signal_check_type_, __LINE__)

CHECK(pthread_t);
CHECK(size_t);
CHECK(uid_t);
CHECK(struct timespec);
CHECK(sig_atomic_t);
CHECK(sigset_t);
CHECK(pid_t);
CHECK(pthread_attr_t);
CHECK(union sigval);
CHECK(struct sigevent);
CHECK(struct sigaction);
CHECK(mcontext_t);
CHECK(ucontext_t);
CHECK(stack_t);
CHECK(siginfo_t);

#undef CHECK
#define CHECK(type) void JOIN(signal_check_fields_, __LINE__)(type *s)

CHECK(union sigval)
{
    s->sival_int = 0;
    s->sival_ptr = (void*)0;
}

CHECK(struct sigevent)
{
    typedef void (*sigev_notify_function_t)(union sigval);

    s->sigev_notify = 0;
    s->sigev_signo  = 0;
    s->sigev_value.sival_int = 0;
    s->sigev_value.sival_ptr = (void*)0;
    s->sigev_notify_function = (sigev_notify_function_t)0;
    s->sigev_notify_attributes = (pthread_attr_t*)0;
}

CHECK(struct sigaction)
{
    typedef void (*sa_handler_t)(int);
    typedef void (*sa_sigaction_t)(int, siginfo_t *, void *);

    s->sa_handler   = (sa_handler_t)0;
    s->sa_flags     = 0;
    s->sa_sigaction = (sa_sigaction_t)0;

    memset(&s->sa_mask, 0, sizeof(s->sa_mask));
}

CHECK(ucontext_t)
{
    s->uc_link = (ucontext_t*)0;

    stack_t *p = &s->uc_stack;
    (void)p;

    memset(&s->uc_sigmask, 0, sizeof(s->uc_sigmask));
    memset(&s->uc_mcontext, 0, sizeof(s->uc_mcontext));
}

CHECK(stack_t)
{
    s->ss_sp    = (void*)0;
    s->ss_size  = (size_t)0;
    s->ss_flags = 0;
}

CHECK(siginfo_t)
{
    s->si_signo  = 0;
    s->si_code   = 0;
#if __XSI_VISIBLE
    s->si_errno  = 0;
#endif
    s->si_pid    = (pid_t)0;
    s->si_uid    = (uid_t)0;
    s->si_addr   = (void*)0;
    s->si_status = 0;

    union sigval *p = &s->si_value;
    (void)p;
}

void signal_check_functions(pthread_t tid, sigset_t *ss
#if _POSIX_REALTIME_SIGNALS > 0
        , union sigval sv
#endif
        )
{
    typedef void (*signal_cb_t)(int );

    (void)kill((pid_t)0, 0);
#if __XSI_VISIBLE
    (void)killpg((pid_t)0, 0);
#endif
#if !__APPLE__
    (void)psiginfo((const siginfo_t *)0, (const char *)0);
#endif
    (void)psignal(0, (const char*)0);
    (void)pthread_kill(tid, 0);
    (void)pthread_sigmask(0, (const sigset_t*)0, (sigset_t*)0);
    (void)raise(0);
    (void)sigaction(0, (const struct sigaction*)0, (struct sigaction*)0);
    (void)sigaddset(ss, 0);
#if __XSI_VISIBLE
    (void)sigaltstack((const stack_t*)0, (stack_t*)0);
#endif
    (void)sigdelset(ss, 0);
    (void)sigemptyset(ss);
    (void)sigfillset(ss);
    (void)sigismember(ss, 0);
    (void)signal(0, (signal_cb_t)0);
    (void)sigpending(ss);
    (void)sigprocmask(0, (const sigset_t*)0, (sigset_t*)0);
#if _POSIX_REALTIME_SIGNALS > 0
    (void)sigqueue((pid_t)0, 0, sv);
#endif
    (void)sigsuspend(ss);
#if _POSIX_REALTIME_SIGNALS > 0
    (void)sigtimedwait((const sigset_t*)0, (siginfo_t*)0, (const struct timespec*)0);
#endif
    (void)sigwait(ss, (int*)1234);
#if _POSIX_REALTIME_SIGNALS > 0
    (void)sigwaitinfo(ss, (siginfo_t*)0);
#endif
}
