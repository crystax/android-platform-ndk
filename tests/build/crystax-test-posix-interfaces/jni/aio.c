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

#if __ANDROID__

/* WARNING!!! Android doesn't include aio.h header */

#else /* !__ANDROID__ */
#include <aio.h>
#if __linux__ && !__ANDROID__
#include <pthread.h> /* for pthread_attr_t */
#endif

#include "gen/aio.inc"

#include "helper.h"

#define CHECK(type) type JOIN(aio_check_type_, __LINE__)

CHECK(struct aiocb);
CHECK(off_t);
CHECK(pthread_attr_t);
CHECK(size_t);
CHECK(ssize_t);
CHECK(struct timespec);
CHECK(struct sigevent *);

void aio_check_aiocb_fields(struct aiocb *s)
{
    s->aio_fildes     = 0;
    s->aio_offset     = (off_t)0;
    s->aio_buf        = (void *)0;
    s->aio_nbytes     = (size_t)0;
    s->aio_reqprio    = 0;
    s->aio_lio_opcode = 0;

    struct sigevent *p = &s->aio_sigevent;
    (void)p;
}

void aio_check_functions(struct aiocb *ai)
{
    (void)aio_cancel(0, ai);
    (void)aio_error(ai);
#if _POSIX_FSYNC > 0 || _POSIX_SYNCHRONIZED_IO > 0
    (void)aio_fsync(0, ai);
#endif
    (void)aio_read(ai);
    (void)aio_return(ai);
    (void)aio_suspend((const struct aiocb * const *)1234, 0, (struct timespec *)0);
    (void)aio_write(ai);
    (void)lio_listio(0, (struct aiocb * const *)1234, 0, (struct sigevent *)0);
}

#endif /* !__ANDROID__ */

