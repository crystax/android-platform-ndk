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

#if _POSIX_MESSAGE_PASSING > 0

#include <mqueue.h>

#include "helper.h"

#define CHECK(type) type JOIN(mqueue_check_type_, __LINE__)

CHECK(mqd_t);
CHECK(pthread_attr_t);
CHECK(size_t);
CHECK(ssize_t);
CHECK(struct timespec);
CHECK(struct sigevent *);
CHECK(struct mq_attr);

void mqueue_check_mq_attr_fields(struct mq_attr *s)
{
    s->mq_flags   = 0;
    s->mq_maxmsg  = 0;
    s->mq_msgsize = 0;
    s->mq_curmsgs = 0;
}

void mqueue_check_functions(mqd_t m)
{
    (void)mq_close(m);
    (void)mq_getattr(m, (struct mq_attr *)0);
    (void)mq_notify(m, (const struct sigevent *)0);
    (void)mq_open((const char *)0, 0, 0);
    (void)mq_receive(m, (char*)0, (size_t)0, (unsigned*)0);
    (void)mq_send(m, (const char *)0, (size_t)0, (unsigned)0);
    (void)mq_setattr(m, (const struct mq_attr *)0, (struct mq_attr *)0);
    (void)mq_timedreceive(m, (char*)0, (size_t)0, (unsigned*)0, (const struct timespec*)0);
    (void)mq_timedsend(m, (const char *)0, (size_t)0, (unsigned)0, (const struct timespec*)0);
    (void)mq_unlink((const char *)0);
}

#endif /* _POSIX_MESSAGE_PASSING > 0 */
