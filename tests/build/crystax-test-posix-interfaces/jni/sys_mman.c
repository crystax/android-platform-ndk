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

#include <sys/mman.h>

#include "gen/sys_mman.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_mman_check_type_, __LINE__)

CHECK(mode_t);
CHECK(off_t);
CHECK(size_t);

#if _POSIX_TYPED_MEMORY_OBJECTS > 0
CHECK(struct posix_typed_mem_info);

void sys_mman_check_posix_typed_mem_info_fields(struct posix_typed_mem_info *s)
{
    s->posix_tmi_length = (size_t)0;
}
#endif /* _POSIX_TYPED_MEMORY_OBJECTS > 0 */

void sys_mman_check_functions()
{
#if _POSIX_MEMLOCK_RANGE > 0
    (void)mlock((const void *)0, (size_t)0);
#endif
#if _POSIX_MEMLOCK > 0
    (void)mlockall(0);
#endif
    (void)mmap((void*)0, (size_t)0, 0, 0, 0, (off_t)0);
    (void)mprotect((void*)0, (size_t)0, 0);
#if __XSI_VISIBLE || _POSIX_SYNCHRONIZED_IO > 0
    (void)msync((void*)0, (size_t)0, 0);
#endif
#if _POSIX_MEMLOCK_RANGE > 0
    (void)munlock((const void*)0, (size_t)0);
#endif
#if _POSIX_MEMLOCK
    (void)munlockall();
#endif
    (void)munmap((void*)0, (size_t)0);
#if _POSIX_ADVISORY_INFO > 0
    (void)posix_madvise((void*)0, (size_t)0, 0);
#endif
#if _POSIX_TYPED_MEMORY_OBJECTS > 0
    (void)posix_mem_offset((const void*)0, (size_t)0, (off_t*)0, (size_t*)0, (int*)0);
    (void)posix_typed_mem_get_info(0, (struct posix_typed_mem_info *)0);
    (void)posix_typed_mem_open((const char *)0, 0, 0);
#endif
#if _POSIX_SHARED_MEMORY_OBJECTS > 0
    (void)shm_open((const char *)0, 0, (mode_t)0);
    (void)shm_unlink((const char *)0);
#endif
}
