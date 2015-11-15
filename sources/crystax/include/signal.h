/*
 * Copyright (c) 2011-2015 CrystaX .NET.
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

#ifndef __CRYSTAX_SIGNAL_H_72db9f389990469a95e32248bf672022
#define __CRYSTAX_SIGNAL_H_72db9f389990469a95e32248bf672022

#include <crystax/id.h>
#include <crystax/google/signal.h>

#include <android/api-level.h>
#include <asm/sigcontext.h>

#ifndef _STRUCT_TIMESPEC
#include <sys/_types.h>
struct timespec {
    __time_t tv_sec;
    long     tv_nsec;
};
#define _STRUCT_TIMESPEC
#endif

__BEGIN_DECLS

int pthread_kill(pthread_t tid, int sig);
int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);

#if __ANDROID_API__ < 8
extern const char * const sys_signame[];

int killpg(int pgrp, int sig);
int sigaltstack(const stack_t *ss, stack_t *oss);

#endif /* __ANDROID_API__ < 8 */

__END_DECLS

#endif /* __CRYSTAX_SIGNAL_H_72db9f389990469a95e32248bf672022 */
