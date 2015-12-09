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

#include <sys/types.h>
#include "helper.h"

#define CHECK(type) type JOIN(sys_types_check_type_, __LINE__)

CHECK(blkcnt_t);
CHECK(blksize_t);
#if __XSI_VISIBLE
CHECK(clock_t);
#endif
#if _POSIX_TIMERS > 0
CHECK(clockid_t);
#endif
CHECK(dev_t);
#if __XSI_VISIBLE
CHECK(fsblkcnt_t);
CHECK(fsfilcnt_t);
#endif
CHECK(gid_t);
CHECK(id_t);
CHECK(ino_t);
CHECK(key_t);
CHECK(mode_t);
CHECK(nlink_t);
CHECK(off_t);
CHECK(pid_t);
#if !__ANDROID__
/* In Android, pthread types defined in pthread.h */
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
#endif /* !__ANDROID__ */
CHECK(size_t);
CHECK(ssize_t);
#if __XSI_VISIBLE
CHECK(suseconds_t);
#endif
CHECK(time_t);
#if _POSIX_TIMERS > 0
CHECK(timer_t);
#endif
#if _POSIX_TRACE > 0
CHECK(trace_attr_t);
CHECK(trace_event_id_t);
#if _POSIX_TRACE_EVENT_FILTER > 0
CHECK(trace_event_set_t);
#endif
CHECK(trace_id_t);
#endif
CHECK(uid_t);
#if __XSI_VISIBLE
CHECK(useconds_t);
#endif
