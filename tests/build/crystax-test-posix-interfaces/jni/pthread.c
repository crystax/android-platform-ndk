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

#include <pthread.h>

#if __ANDROID__
#include <android/api-level.h>
#endif

/* We don't support Android API level < 9 */
#if !__ANDROID__ || __ANDROID_API__ >= 9

#include "gen/pthread.inc"

#include "helper.h"

#define CHECK(type) type JOIN(pthread_check_type_, type);

CHECK(pthread_attr_t);
#if _POSIX_BARRIERS > 0
CHECK(pthread_barrier_t);
CHECK(pthread_barrierattr_t);
#endif
CHECK(pthread_cond_t);
CHECK(pthread_condattr_t);
CHECK(pthread_key_t);
CHECK(pthread_mutex_t);
CHECK(pthread_mutexattr_t);
CHECK(pthread_once_t);
CHECK(pthread_rwlock_t);
CHECK(pthread_rwlockattr_t);
#if _POSIX_SPIN_LOCKS > 0
CHECK(pthread_spinlock_t);
#endif
CHECK(pthread_t);

#ifndef pthread_cleanup_push
#error pthread_cleanup_push not defined
#endif

#ifndef pthread_cleanup_pop
#error pthread_cleanup_pop not defined
#endif

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

int pthread_atfork(void (*f1)(void), void (*f2)(void), void(*f3)(void))
{
    (void)f1;
    (void)f2;
    (void)f3;
    return 1234;
}

int pthread_attr_getinheritsched(const pthread_attr_t *a, int *p)
{
    (void)a;
    (void)p;
    return 1234;
}

int pthread_attr_setinheritsched(pthread_attr_t *a, int p)
{
    (void)a;
    (void)p;
    return 1234;
}

#if __ANDROID_API__ < 21
int pthread_condattr_getclock(const pthread_condattr_t *a, clockid_t *t)
{
    (void)a;
    (void)t;
    return 1234;
}

int pthread_condattr_setclock(pthread_condattr_t *a, clockid_t t)
{
    (void)a;
    (void)t;
    return 1234;
}
#endif /* __ANDROID_API__ < 21 */

int pthread_cancel(pthread_t tid)
{
    (void)tid;
    return 1234;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    (void)state;
    (void)oldstate;
    return 1234;
}

int pthread_setcanceltype(int type, int *oldtype)
{
    (void)type;
    (void)oldtype;
    return 1234;
}

void pthread_testcancel()
{
}

int pthread_mutex_getprioceiling(const pthread_mutex_t *m, int *p)
{
    (void)m;
    (void)p;
    return 1234;
}

int pthread_mutex_setprioceiling(pthread_mutex_t *m, int c, int *p)
{
    (void)m;
    (void)c;
    (void)p;
    return 1234;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *a, int *p)
{
    (void)a;
    (void)p;
    return 1234;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *a, int c)
{
    (void)a;
    (void)c;
    return 1234;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *a, int *p)
{
    (void)a;
    (void)p;
    return 1234;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *a, int c)
{
    (void)a;
    (void)c;
    return 1234;
}

int pthread_setschedprio(pthread_t t, int p)
{
    (void)t;
    (void)p;
    return 1234;
}

#endif /* __ANDROID__ */

void *pthread_new_thread(void *arg)
{
    return arg;
}

void pthread_once_action()
{
}

void pthread_check_functions()
{
    pthread_t tid;
    pthread_attr_t attr;
    int state;
    struct sched_param sched_param_value;
    pthread_cond_t cond;
    pthread_condattr_t cattr;
    pthread_mutex_t mtx;
    pthread_mutexattr_t mattr;
    struct timespec ts;
    pthread_key_t key;
    pthread_once_t once;
    pthread_rwlock_t rwlock;
    pthread_rwlockattr_t rwattr;

#if _POSIX_BARRIERS > 0
    pthread_barrier_t barr;
    pthread_barrierattr_t battr;
#endif

#if _POSIX_CLOCK_SELECTION > 0
    clockid_t clockid;
#endif

#if _POSIX_SPIN_LOCKS > 0
    pthread_spinlock_t spin;
#endif

    tid = pthread_self();
    (void)tid;

    (void)pthread_atfork(NULL, NULL, NULL);
    (void)pthread_attr_destroy(&attr);
    (void)pthread_attr_getdetachstate(&attr, &state);
#if __XSI_VISIBLE
    (void)pthread_attr_getguardsize(&attr, (size_t*)1234);
#endif
#if _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)pthread_attr_getinheritsched(&attr, (int*)1234);
#endif
    (void)pthread_attr_getschedparam(&attr, &sched_param_value);
#if _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)pthread_attr_getschedpolicy(&attr, (int*)1234);
    (void)pthread_attr_getscope(&attr, (int*)1234);
#endif
#if _POSIX_THREAD_ATTR_STACKADDR > 0 || _POSIX_THREAD_ATTR_STACKSIZE > 0
    (void)pthread_attr_getstack(&attr, (void**)1234, (size_t*)1234);
#endif
#if _POSIX_THREAD_ATTR_STACKSIZE > 0
    (void)pthread_attr_getstacksize(&attr, (size_t*)1234);
#endif
    (void)pthread_attr_init(&attr);
    (void)pthread_attr_setdetachstate(&attr, 0);
#if __XSI_VISIBLE
    (void)pthread_attr_setguardsize(&attr, 0);
#endif
#if _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)pthread_attr_setinheritsched(&attr, 0);
#endif
    (void)pthread_attr_setschedparam(&attr, &sched_param_value);
#if _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)pthread_attr_setschedpolicy(&attr, 0);
    (void)pthread_attr_setscope(&attr, 0);
#endif
#if _POSIX_THREAD_ATTR_STACKADDR > 0 || _POSIX_THREAD_ATTR_STACKSIZE > 0
    (void)pthread_attr_setstack(&attr, (void*)1234, (size_t)1234);
#endif
#if _POSIX_THREAD_ATTR_STACKSIZE > 0
    (void)pthread_attr_setstacksize(&attr, (size_t)1234);
#endif
#if _POSIX_BARRIERS > 0
    (void)pthread_barrier_destroy(&barr);
    (void)pthread_barrier_init(&barr, &battr, 0);
    (void)pthread_barrier_wait(&barr);
    (void)pthread_barrierattr_destroy(&battr);
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_barrierattr_getpshared(&battr, NULL);
#endif
    (void)pthread_barrierattr_init(&battr);
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_barrierattr_setpshared(&battr, 0);
#endif
#endif /* _POSIX_BARRIERS > 0 */
    (void)pthread_cancel(tid);
    (void)pthread_cond_broadcast(&cond);
    (void)pthread_cond_destroy(&cond);
    (void)pthread_cond_init(&cond, &cattr);
    (void)pthread_cond_signal(&cond);
    (void)pthread_cond_timedwait(&cond, &mtx, &ts);
    (void)pthread_cond_wait(&cond, &mtx);
    (void)pthread_condattr_destroy(&cattr);
#if _POSIX_CLOCK_SELECTION > 0
    (void)pthread_condattr_getclock(&cattr, &clockid);
#endif
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_condattr_getpshared(&cattr, NULL);
#endif
    (void)pthread_condattr_init(&cattr);
#if _POSIX_CLOCK_SELECTION > 0
    (void)pthread_condattr_setclock(&cattr, clockid);
#endif
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_condattr_setpshared(&cattr, 0);
#endif
    (void)pthread_create(&tid, &attr, pthread_new_thread, NULL);
    (void)pthread_detach(tid);
    (void)pthread_equal(tid, tid);
    (void)pthread_exit(NULL);
#if _POSIX_THREAD_CPUTIME > 0
    (void)pthread_getcpuclockid(tid, &clockid);
#endif
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_getschedparam(tid, (int*)123456, &sched_param_value);
#endif
    (void)pthread_getspecific(key);
    (void)pthread_join(tid, NULL);
    (void)pthread_key_create(&key, NULL);
    (void)pthread_key_delete(key);
    (void)pthread_mutex_destroy(&mtx);
#if _POSIX_THREAD_PRIO_PROTECT > 0
    (void)pthread_mutex_getprioceiling(&mtx, NULL);
#endif
    (void)pthread_mutex_init(&mtx, &mattr);
    (void)pthread_mutex_lock(&mtx);
#if _POSIX_THREAD_PRIO_PROTECT > 0
    (void)pthread_mutex_setprioceiling(&mtx, 0, NULL);
#endif
#if _POSIX_TIMEOUTS > 0
    (void)pthread_mutex_timedlock(&mtx, &ts);
#endif
    (void)pthread_mutex_trylock(&mtx);
    (void)pthread_mutex_unlock(&mtx);
    (void)pthread_mutexattr_destroy(&mattr);
#if _POSIX_THREAD_PRIO_PROTECT > 0
    (void)pthread_mutexattr_getprioceiling(&mattr, NULL);
#if _POSIX_THREAD_PRIO_INHERIT > 0
    (void)pthread_mutexattr_getprotocol(&mattr, NULL);
#endif
#endif
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_mutexattr_getpshared();
#endif
#if __XSI_VISIBLE
    (void)pthread_mutexattr_gettype(&mattr, (int*)1234);
#endif
    (void)pthread_mutexattr_init(&mattr);
#if _POSIX_THREAD_PRIO_PROTECT > 0
    (void)pthread_mutexattr_setprioceiling(&mattr, 0);
#if _POSIX_THREAD_PRIO_INHERIT > 0
    (void)pthread_mutexattr_setprotocol(&mattr, 0);
#endif
#endif
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_mutexattr_setpshared(&mattr, 0);
#endif
#if __XSI_VISIBLE
    (void)pthread_mutexattr_settype(&mattr, 0);
#endif
    (void)pthread_once(&once, pthread_once_action);
    (void)pthread_rwlock_destroy(&rwlock);
    (void)pthread_rwlock_init(&rwlock, &rwattr);
    (void)pthread_rwlock_rdlock(&rwlock);
#if _POSIX_TIMEOUTS > 0
    (void)pthread_rwlock_timedrdlock(&rwlock, &ts);
    (void)pthread_rwlock_timedwrlock(&rwlock, &ts);
#endif
    (void)pthread_rwlock_tryrdlock(&rwlock);
    (void)pthread_rwlock_trywrlock(&rwlock);
    (void)pthread_rwlock_unlock(&rwlock);
    (void)pthread_rwlock_wrlock(&rwlock);
    (void)pthread_rwlockattr_destroy(&rwattr);
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_rwlockattr_getpshared(&rwattr, NULL);
#endif
    (void)pthread_rwlockattr_init(&rwattr);
#if _POSIX_THREAD_PROCESS_SHARED > 0
    (void)pthread_rwlockattr_setpshared(&rwattr, 0);
#endif
    (void)pthread_self();
    (void)pthread_setcancelstate(0, NULL);
    (void)pthread_setcanceltype(0, NULL);
#if _POSIX_THREAD_PRIORITY_SCHEDULING > 0
    (void)pthread_setschedparam(tid, 0, &sched_param_value);
    (void)pthread_setschedprio(tid, 0);
#endif
    (void)pthread_setspecific(key, NULL);
#if _POSIX_SPIN_LOCKS > 0
    (void)pthread_spin_destroy(&spin);
    (void)pthread_spin_init(&spin, 0);
    (void)pthread_spin_lock(&spin);
    (void)pthread_spin_trylock(&spin);
    (void)pthread_spin_unlock(&spin);
#endif /* _POSIX_SPIN_LOCKS > 0 */
    (void)pthread_testcancel();
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 9 */
