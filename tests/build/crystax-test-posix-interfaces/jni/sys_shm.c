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

#include <sys/shm.h>

#include "gen/sys_shm.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_shm_check_type_, __LINE__)

#if !__ANDROID__
CHECK(shmatt_t);
#endif /* !__ANDROID__ */
CHECK(struct shmid_ds);
CHECK(pid_t);
CHECK(size_t);
CHECK(time_t);

#if __ANDROID__
void *shmat(int shmid, const void *shmaddr, int shmflg)
{
    (void)shmid;
    (void)shmaddr;
    (void)shmflg;
    return (void*)12345;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
    (void)shmid;
    (void)cmd;
    (void)buf;
    return -1;
}

int shmdt(const void *shmaddr)
{
    (void)shmaddr;
    return -1;
}

int shmget(key_t key, size_t size, int shmflg)
{
    (void)key;
    (void)size;
    (void)shmflg;
    return -1;
}
#endif /* __ANDROID__ */

void sys_shm_check_functions()
{
    (void)shmat(0, (const void*)1234, 0);
    (void)shmctl(0, 0, (struct shmid_ds *)1234);
    (void)shmdt((const void *)1234);
    (void)shmget((key_t)0, (size_t)0, 0);
}
