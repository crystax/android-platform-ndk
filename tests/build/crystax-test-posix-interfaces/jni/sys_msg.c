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

#if __ANDROID__
/* WARNING!!! Android have no support for sys/msg.h */

#else /* !__ANDROID__ */

#include <sys/msg.h>

#ifndef MSG_NOERROR
#error 'MSG_NOERROR' not defined
#endif

#include "helper.h"

#define CHECK(type) type JOIN(sys_msg_check_type_, __LINE__)

CHECK(msgqnum_t);
CHECK(msglen_t);
CHECK(struct msqid_ds);

void sys_msg_check_msqid_ds_fields(struct msqid_ds *s)
{
    struct ipc_perm *p = &s->msg_perm;
    (void)p;

    s->msg_qnum   = (msgqnum_t)0;
    s->msg_qbytes = (msglen_t)0;
    s->msg_lspid  = (pid_t)0;
    s->msg_lrpid  = (pid_t)0;
    s->msg_stime  = (time_t)0;
    s->msg_rtime  = (time_t)0;
    s->msg_ctime  = (time_t)0;
}

void sys_msg_check_functions()
{
    (void)msgctl(0, 0, (struct msqid_ds*)0);
    (void)msgget((key_t)0, 0);
    (void)msgrcv(0, (void*)0, (size_t)0, (long)0, 0);
    (void)msgsnd(0, (const void*)0, (size_t)0, 0);
}

#endif /* !__ANDROID__ */
