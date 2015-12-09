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
#include <android/api-level.h>
#endif

#if !__ANDROID__ || __ANDROID_API__ >= 9

#include <unistd.h>

#if __APPLE__
#define CRYSTAX_POSIX_VERSION 200112L
#define CRYSTAX_XOPEN_VERSION 600
#elif __gnu_linux__
#define CRYSTAX_POSIX_VERSION 200809L
#define CRYSTAX_XOPEN_VERSION 700
#elif __ANDROID__
#define CRYSTAX_POSIX_VERSION 200809L
#define CRYSTAX_XOPEN_VERSION 700
#else
#error Not supported platform
#endif

#if !defined(_POSIX_VERSION) || _POSIX_VERSION != CRYSTAX_POSIX_VERSION
#error Wrong _POSIX_VERSION
#endif

#if !defined(_POSIX2_VERSION) || (_POSIX2_VERSION != -1 && _POSIX2_VERSION != CRYSTAX_POSIX_VERSION)
#error Wrong _POSIX2_VERSION
#endif

#if !defined(_XOPEN_VERSION) || _XOPEN_VERSION != CRYSTAX_XOPEN_VERSION
#error Wrong _XOPEN_VERSION
#endif

#include "gen/unistd.inc"

#include "helper.h"

#define CHECK(type) type JOIN(unistd_check_type_, __LINE__)

CHECK(size_t);
CHECK(ssize_t);
CHECK(uid_t);
CHECK(gid_t);
CHECK(off_t);
CHECK(pid_t);
CHECK(intptr_t);

#if __ANDROID__
/* WARNING!!! These functions not defined in Android, so we define empty stubs here */
/* TODO: implement them in libcrystax */

size_t confstr(int name, char *buf, size_t len)
{
    (void)name;
    (void)buf;
    (void)len;
    return 0;
}

char *crypt(const char *key, const char *salt)
{
    (void)key;
    (void)salt;
    return (char*)0;
}

void encrypt(char block[64], int edflag)
{
    (void)block;
    (void)edflag;
}

int faccessat(int fd, const char *path, int amode, int flag)
{
    (void)fd;
    (void)path;
    (void)amode;
    (void)flag;
    return -1;
}

int fchownat(int fd, const char *path, uid_t owner, gid_t group, int flag)
{
    (void)fd;
    (void)path;
    (void)owner;
    (void)group;
    (void)flag;
    return -1;
}

int fexecve(int fd, char *const argv[], char *const envp[])
{
    (void)fd;
    (void)argv;
    (void)envp;
    return -1;
}

long gethostid()
{
    return 0;
}

int getlogin_r(char *name, size_t namesize)
{
    (void)name;
    (void)namesize;
    return -1;
}

pid_t getsid(pid_t pid)
{
    (void)pid;
    return (pid_t)-1;
}

int linkat(int fd1, const char *path1, int fd2,
           const char *path2, int flag)
{
    (void)fd1;
    (void)path1;
    (void)fd2;
    (void)path2;
    (void)flag;
    return -1;
}

ssize_t readlinkat(int fd, const char *path,
                   char *buf, size_t bufsize)
{
    (void)fd;
    (void)path;
    (void)buf;
    (void)bufsize;
    return -1;
}

int symlinkat(const char *path1, int fd, const char *path2)
{
    (void)path1;
    (void)fd;
    (void)path2;
    return -1;
}

int unlinkat(int fd, const char *path, int flag)
{
    (void)fd;
    (void)path;
    (void)flag;
    return -1;
}

#endif /* __ANDROID__ */

void unistd_check_functions()
{
    (void)access("", 0);
    (void)alarm((unsigned)0);
    (void)chdir("");
    (void)chown("", (uid_t)0, (gid_t)0);
    (void)close(0);
    (void)confstr(0, (char*)1234, (size_t)0);
    (void)crypt("", "");
    (void)dup(0);
    (void)dup2(0, 0);
    (void)_exit(0);
    (void)encrypt((char*)1234, 0);
    (void)execl("", "", NULL);
    (void)execle("", "", NULL, (char * const *)1234);
    (void)execlp("", "", NULL);
    (void)execv("", (char * const *)1234);
    (void)execve("", (char * const *)1234, (char * const *)1234);
    (void)execvp("", (char * const *)1234);
#if HAVE_XXXAT
    (void)faccessat(0, "", 0, 0);
#endif
    (void)fchdir(0);
    (void)fchown(0, (uid_t)0, (gid_t)0);
#if HAVE_XXXAT
    (void)fchownat(0, "", (uid_t)0, (gid_t)0, 0);
#endif
#if _POSIX_SYNCHRONIZED_IO > 0
    (void)fdatasync(0);
#endif
#if !__APPLE__
    (void)fexecve(0, (char * const *)1234, (char * const *)1234);
#endif
    (void)fork();
    (void)fpathconf(0, 0);
#if _POSIX_FSYNC > 0
    (void)fsync(0);
#endif
    (void)ftruncate(0, (off_t)0);
    (void)getcwd((char*)1234, (size_t)0);
    (void)getegid();
    (void)geteuid();
    (void)getgid();
    (void)getgroups(0, (gid_t*)0);
    (void)gethostid();
    (void)gethostname((char*)1234, (size_t)0);
    (void)getlogin();
    (void)getlogin_r((char*)1234, (size_t)0);
    (void)getopt(0, (char * const *)1234, "");
    (void)getpgid((pid_t)0);
    (void)getpgrp();
    (void)getpid();
    (void)getppid();
    (void)getsid((pid_t)0);
    (void)getuid();
    (void)isatty(0);
    (void)lchown("", (uid_t)0, (gid_t)0);
    (void)link("", "");
#if HAVE_XXXAT
    (void)linkat(0, "", 0, "", 0);
#endif
    (void)lockf(0, 0, (off_t)0);
    (void)lseek(0, (off_t)0, 0);
    (void)nice(0);
    (void)pathconf("", 0);
    (void)pause();
    (void)pipe((int*)1234);
    (void)pread(0, (void*)1234, (size_t)0, (off_t)0);
    (void)pwrite(0, (const void *)1234, (size_t)0, (off_t)0);
    (void)read(0, (void*)1234, (size_t)0);
    (void)readlink("", (char*)1234, (size_t)0);
#if HAVE_XXXAT
    (void)readlinkat(0, "", (char*)1234, (size_t)0);
#endif
    (void)rmdir("");
    (void)setegid((gid_t)0);
    (void)seteuid((uid_t)0);
    (void)setgid((gid_t)0);
    (void)setpgid((pid_t)0, (pid_t)0);
    (void)setpgrp();
    (void)setregid((gid_t)0, (gid_t)0);
    (void)setreuid((uid_t)0, (uid_t)0);
    (void)setsid();
    (void)setuid((uid_t)0);
    (void)sleep((unsigned)0);
    (void)swab((const void *)1234, (void*)1234, (size_t)0);
    (void)symlink("", "");
#if HAVE_XXXAT
    (void)symlinkat("", 0, "");
#endif
    (void)sync();
    (void)sysconf(0);
    (void)tcgetpgrp(0);
    (void)tcsetpgrp(0, (pid_t)0);
    (void)truncate("", (off_t)0);
    (void)ttyname(0);
    (void)ttyname_r(0, (char*)1234, (size_t)0);
    (void)unlink("");
#if HAVE_XXXAT
    (void)unlinkat(0, "", 0);
#endif
    (void)write(0, (const void *)1234, (size_t)0);
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 9 */
