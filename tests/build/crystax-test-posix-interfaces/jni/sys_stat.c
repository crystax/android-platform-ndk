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

#include <sys/stat.h>

#include "gen/sys_stat.inc"

#include "helper.h"

#define CHECK(type) type JOIN(sys_stat_check_type_, __LINE__)

CHECK(struct stat);
CHECK(blkcnt_t);
CHECK(blksize_t);
CHECK(dev_t);
CHECK(ino_t);
CHECK(mode_t);
CHECK(nlink_t);
CHECK(uid_t);
CHECK(gid_t);
CHECK(off_t);
CHECK(time_t);
CHECK(struct timespec);

void sys_stat_timespec_helper(struct timespec *ts)
{
    (void)ts;
}

void sys_stat_check_stat_fields(struct stat *s)
{
    s->st_dev     = (dev_t)0;
    s->st_ino     = (ino_t)0;
    s->st_mode    = (mode_t)0;
    s->st_nlink   = (nlink_t)0;
    s->st_uid     = (uid_t)0;
    s->st_gid     = (gid_t)0;
    s->st_rdev    = (dev_t)0;
    s->st_size    = (off_t)0;
    s->st_blksize = (blksize_t)0;
    s->st_blocks  = (blkcnt_t)0;

#if __ANDROID__
    s->st_atime     = (time_t)0;
    s->st_atimensec = (long)0;
    s->st_mtime     = (time_t)0;
    s->st_mtimensec = (long)0;
    s->st_ctime     = (time_t)0;
    s->st_ctimensec = (long)0;
#elif __APPLE__
    sys_stat_timespec_helper(&s->st_atimespec);
    sys_stat_timespec_helper(&s->st_mtimespec);
    sys_stat_timespec_helper(&s->st_ctimespec);
#else
    sys_stat_timespec_helper(&s->st_atim);
    sys_stat_timespec_helper(&s->st_mtim);
    sys_stat_timespec_helper(&s->st_ctim);
#endif
}

#if __ANDROID__ || __APPLE__
int futimens(int x, const struct timespec y[2])
{
    (void)x;
    (void)y;
    return -1;
}

int mkfifoat(int f, const char *s, mode_t m)
{
    (void)f;
    (void)s;
    (void)m;
    return -1;
}

int mknodat(int f, const char *s, mode_t m, dev_t d)
{
    (void)f;
    (void)s;
    (void)m;
    (void)d;
    return -1;
}

int utimensat(int f, const char *s, const struct timespec t[2], int g)
{
    (void)f;
    (void)s;
    (void)t;
    (void)g;
    return -1;
}
#endif /* __ANDROID__ || __APPLE__ */

void sys_stat_check_functions()
{
    (void)chmod((const char *)1234, (mode_t)0);
    (void)fchmod(0, (mode_t)0);
#if HAVE_XXXAT
    (void)fchmodat(0, (const char *)1234, (mode_t)0, 0);
#endif
    (void)fstat(0, (struct stat *)1234);
#if HAVE_XXXAT
    (void)fstatat(0, (const char *)1234, (struct stat *)1234, 0);
#endif
    (void)futimens(0, (const struct timespec *)1234);
    (void)lstat((const char *)1234, (struct stat *)1234);
    (void)mkdir((const char *)1234, (mode_t)0);
#if HAVE_XXXAT
    (void)mkdirat(0, (const char *)1234, (mode_t)0);
#endif
    (void)mkfifo((const char *)1234, (mode_t)0);
    (void)mkfifoat(0, (const char *)1234, (mode_t)0);
    (void)mknod((const char *)1234, (mode_t)0, (dev_t)0);
    (void)mknodat(0, (const char *)1234, (mode_t)0, (dev_t)0);
    (void)stat((const char *)1234, (struct stat *)1234);
    (void)umask((mode_t)0);
    (void)utimensat(0, (const char *)1234, (const struct timespec *)1234, 0);
}
