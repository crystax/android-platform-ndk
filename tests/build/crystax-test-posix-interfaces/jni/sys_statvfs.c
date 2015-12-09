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
/* WARNING!!! sys/statvfs.h appears in Android starting from 21 API level */
/* TODO: check it and see if it's possible implement the same interface for older Android versions */

#else /* !__ANDROID__ */

#include <sys/statvfs.h>

#include "gen/sys_statvfs.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_statvfs_check_type_, __LINE__)

CHECK(struct statvfs);
CHECK(fsblkcnt_t);
CHECK(fsfilcnt_t);

void sys_statvfs_check_statvfs_fields(struct statvfs *s)
{
    s->f_bsize   = (unsigned long)0;
    s->f_frsize  = (unsigned long)0;
    s->f_blocks  = (fsblkcnt_t)0;
    s->f_bfree   = (fsblkcnt_t)0;
    s->f_bavail  = (fsblkcnt_t)0;
    s->f_files   = (fsfilcnt_t)0;
    s->f_ffree   = (fsfilcnt_t)0;
    s->f_favail  = (fsfilcnt_t)0;
    s->f_fsid    = (unsigned long)0;
    s->f_flag    = (unsigned long)0;
    s->f_namemax = (unsigned long)0;
}

void sys_statvfs_check_functions(struct statvfs *s)
{
    (void)fstatvfs(0, s);
    (void)statvfs("path", s);
}

#endif /* !__ANDROID__ */
