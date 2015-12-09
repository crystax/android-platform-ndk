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

#include <semaphore.h>

#include "helper.h"

#define CHECK(type) type JOIN(semaphore_check_type_, __LINE__)

CHECK(sem_t);

#if !defined(SEM_FAILED)
#error 'SEM_FAILED' not defined
#endif

sem_t *semaphore_check_SEM_FAILED = SEM_FAILED;

void semaphore_check_functions(sem_t *s)
{
    (void)sem_close(s);
    (void)sem_open((const char*)0, 0, 0);
    (void)sem_post(s);
#if _POSIX_TIMEOUTS > 0
    (void)sem_timedwait(s, (const struct timespec*)0);
#endif
    (void)sem_trywait(s);
    (void)sem_unlink((const char*)0);
    (void)sem_wait(s);

#if __APPLE__ && defined(__MAC_10_8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

    (void)sem_destroy(s);
    (void)sem_getvalue(s, (int*)0);
    (void)sem_init(s, 0, 0);

#if __APPLE__ && defined(__MAC_10_8)
#pragma GCC diagnostic pop
#endif
}
