/*
 * Copyright (c) 2011-2018 CrystaX.
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

#include <crystax/log.h>
#include <crystax.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <kernel/uapi/linux/binfmts.h>

extern int __execve(const char *filename, char * const argv[], char * const envp[]);

static char *extract_shebang(const char *filename)
{
    int fd = -1;
    char *shebang = NULL;
    char buf[BINPRM_BUF_SIZE + 1];
    ssize_t n;
    char *s;
    char *start;

    if ((fd = open(filename, O_RDONLY)) < 0)
        return NULL;

    if (read(fd, buf, 2) != 2)
        goto extract_shebang_exit;

    if (buf[0] != '#' && buf[1] != '!')
        goto extract_shebang_exit;

    if ((n = read(fd, buf, sizeof(buf) - 1)) < 0)
        goto extract_shebang_exit;
    buf[n] = '\0';

    s = buf;
    for (; *s != '\0' && isspace(*s); ++s);
    if (*s == '\0')
        goto extract_shebang_exit;

    start = s;
    for (; *s != '\0' && *s != '\n'; ++s);
    *s = '\0';

    n = s - start;

    shebang = (char *)malloc(n + 1);
    if (shebang == NULL)
        goto extract_shebang_exit;

    memcpy(shebang, start, n + 1);

extract_shebang_exit:

    close(fd);

    return shebang;
}

static int exec_with_args(char *exe, char *args, char * const argv[], char * const envp[])
{
    size_t newargc;
    char **newargv;
    char * const *p;
    char *s;
    size_t n, i;

    newargc = 1;
    p = argv;
    for (; *p; ++p, ++newargc);

    s = args;
    for (;;) {
        for (; *s != '\0' && isspace(*s); ++s);
        if (*s == '\0')
            break;
        ++newargc;
        for (; *s != '\0' && !isspace(*s); ++s);
        if (*s == '\0')
            break;
        ++s;
    }

    newargv = (char **) malloc((newargc + 1) * sizeof(char *));
    if (newargv == NULL) {
        errno = ENOMEM;
        return -1;
    }

    memset(newargv, 0, (newargc + 1) * sizeof(char *));

    n = 0;
    newargv[n++] = exe;

    s = args;
    for (;;) {
        for (; *s != '\0' && isspace(*s); ++s);
        if (*s == '\0')
            break;
        newargv[n++] = s;
        for (; *s != '\0' && !isspace(*s); ++s);
        if (*s == '\0')
            break;
        *s++ = '\0';
    }

    for (i = n; i < newargc; ++i)
        newargv[i] = argv[i - n];

    return __execve(exe, newargv, envp);
}

static void exec_env(char *args, char * const argv[], char * const envp[])
{
    char *s;
    char *exe;
    size_t exelen, len;
    const char *path, *p, *pend;
    char *newexe;
    int serrno;

    path = getenv("PATH");
    if (path == NULL || strlen(path) == 0)
        return;

    for (s = args; *s != '\0' && isspace(*s); ++s);
    if (*s == '\0')
        return;
    exe = s;

    for (; *s != '\0' && !isspace(*s); ++s);
    if (*s != '\0') {
        *s++ = '\0';
        for (; *s != '\0' && isspace(*s); ++s);
        args = s;
    }

    exelen = strlen(exe);
    if (exelen == 0 || exe[0] == '/')
        return;

    p = path;
    for (;; p = pend + 1) {
        pend = p;
        for (; *pend != '\0' && *pend != ':'; ++pend);
        if (*pend == '\0' || pend == p)
            break;

        len = pend - p + 1 + exelen + 1;
        newexe = (char *)malloc(len);
        if (newexe == NULL)
            return;

        memcpy(newexe, p, pend - p);
        newexe[pend - p] = '\0';
        strcat(newexe, "/");
        strcat(newexe, exe);

        if (access(newexe, R_OK|X_OK) == 0) {
            exec_with_args(newexe, args, argv, envp);

            serrno = errno;
            free(newexe);
            errno = serrno;

            return;
        }

        free(newexe);
    }
}

static int exec_shebang(char *shebang, const char *filename, char * const argv[],
        char * const envp[])
{
    char *s;
    char *exe = NULL, *args = NULL;
    char *newexe = NULL;
    ssize_t n;
    struct stat st;
    int rc, serrno;

    const char *topdir = NULL;

    for (s = shebang; *s != '\0' && isspace(*s); ++s);
    if (*s == '\0')
        return __execve(filename, argv, envp);

    exe = s;

    for (; *s != '\0' && !isspace(*s); ++s);

    args = s;
    if (*s != '\0') {
        *s++ = '\0';
        for (; *s != '\0' && isspace(*s); ++s);
        if (*s != '\0')
            args = s;
    }

    if (*exe != '/')
        return __execve(filename, argv, envp);

    topdir = crystax_posix_base();
    if (topdir == NULL || strlen(topdir) == 0) {
        if (strcmp(exe, "/usr/bin/env") == 0)
            exec_env(args, argv, envp);
        return __execve(filename, argv, envp);
    }

    n = strlen(topdir) + strlen(exe) + 1;
    newexe = (char *)malloc(n);
    if (newexe == NULL) {
        errno = ENOMEM;
        return -1;
    }

    if (snprintf(newexe, n, "%s%s", topdir, exe) == n) {
        free(newexe);
        errno = EFAULT;
        return -1;
    }

    if (stat(newexe, &st) < 0) {
        free(newexe);

        if (errno == ENOENT && strcmp(exe, "/usr/bin/env") == 0)
            exec_env(args, argv, envp);

        return __execve(filename, argv, envp);
    }

    rc = exec_with_args(newexe, args, argv, envp);

    serrno = errno;
    free(newexe);
    errno = serrno;

    return rc;
}

int execve(const char *filename, char * const argv[], char * const envp[])
{
    char *shebang;
    int rc, serrno;

    shebang = extract_shebang(filename);
    if (shebang == NULL)
        return __execve(filename, argv, envp);

    rc = exec_shebang(shebang, filename, argv, envp);
    serrno = errno;
    free(shebang);
    errno = serrno;
    return rc;
}
