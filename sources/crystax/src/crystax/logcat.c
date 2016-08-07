/*
 * Copyright (c) 2011-2016 CrystaX.
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

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <crystax/log.h>

#undef  LOG_TAG
#define LOG_TAG "CRYSTAX"

#undef  PANIC
#define PANIC(fmt, ...) \
    do { \
        __crystax_logcat(CRYSTAX_LOGLEVEL_PANIC, LOG_TAG, fmt, ##__VA_ARGS__); \
        abort(); \
    } while (0)

extern int __crystax_logwrite(int, const char *, const char *);
extern int __cxa_atexit(void (*func)(void *), void *arg, void *dso);

static int handle_input(int fd, int oldfd, int prio, const char *tag)
{
    char buf[4097];
    ssize_t nread;

    for (;;) {
        nread = read(fd, buf, sizeof(buf) - 1);
        if (nread < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (nread == 0)
            return -1;
        if (oldfd >= 0)
            write(oldfd, buf, nread);
        break;
    }
    buf[nread] = '\0';

    __crystax_logwrite(prio, tag, buf);

    return 0;
}

typedef struct stdio2logcat_t_
{
    int ofd;
    int oldofd;
    int efd;
    int oldefd;
} stdio2logcat_t;

static void *stdio2logcat(void *arg)
{
    stdio2logcat_t *ls = (stdio2logcat_t *)arg;
    int ofd    = ls->ofd;
    int oldofd = ls->oldofd;
    int efd    = ls->efd;
    int oldefd = ls->oldefd;

    fd_set fdset;
    int maxfd = ofd < efd ? efd : ofd;

    for (;;) {
        if (ofd < 0 || efd < 0) break;

        FD_ZERO(&fdset);
        if (ofd >= 0) FD_SET(ofd, &fdset);
        if (efd >= 0) FD_SET(efd, &fdset);

        if (select(maxfd + 1, &fdset, NULL, NULL, NULL) < 0) {
            if (errno == EINTR)
                continue;
            break;
        }

        if (FD_ISSET(ofd, &fdset) && handle_input(ofd, oldofd, CRYSTAX_LOGLEVEL_INFO, "STDOUT") < 0) {
            close(ofd);
            if (oldofd >= 0) {
                dup2(oldofd, ofd);
                close(oldofd);
            }
            ofd = -1;
        }

        if (FD_ISSET(efd, &fdset) && handle_input(efd, oldefd, CRYSTAX_LOGLEVEL_ERROR, "STDERR") < 0) {
            close(efd);
            if (oldefd >= 0) {
                dup2(oldefd, efd);
                close(oldefd);
            }
            efd = -1;
        }
    }

    close(ofd);
    close(oldofd);
    close(efd);
    close(oldefd);

    return NULL;
}

static void close_stdio_fds(void *arg)
{
    stdio2logcat_t *ls = (stdio2logcat_t *)arg;
    close(ls->ofd);
    close(ls->oldofd);
    close(ls->efd);
    close(ls->oldefd);
    free(arg);
}

static void set_flags(int fd, int newflags)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) < 0)
        PANIC("can't get pipe's flags: %s", strerror(errno));
    if (fcntl(fd, F_SETFL, flags | newflags) < 0)
        PANIC("can't set pipe's flags: %s", strerror(errno));
}

static int dupfd(int *fds, int fd)
{
    int oldfd;

    if (pipe(fds) < 0)
        PANIC("can't open pipe: %s", strerror(errno));

    oldfd = dup(fd);
    close(fd);
    if (dup2(fds[1], fd) < 0)
        PANIC("can't duplicate pipe to fd %d: %s", fd, strerror(errno));
    if (close(fds[1]) < 0)
        PANIC("can't close duplicated pipe: %s", strerror(errno));

    set_flags(fds[0], O_NONBLOCK | O_CLOEXEC);
    set_flags(fd, O_CLOEXEC);

    return oldfd;
}

void __crystax_redirect_stdio_to_logcat()
{
    int ofd[2], efd[2];
    int oldofd, oldefd;
    int rc;
    pthread_t pth;
    pthread_attr_t pattr;
    struct sched_param sparam;
    int policy;

    oldofd = dupfd(ofd, STDOUT_FILENO);
    oldefd = dupfd(efd, STDERR_FILENO);

    stdio2logcat_t *ls = (stdio2logcat_t*)malloc(sizeof(stdio2logcat_t));
    if (!ls) PANIC("can't allocate memory for pipes: %s", strerror(errno));
    ls->ofd    = ofd[0];
    ls->oldofd = oldofd;
    ls->efd    = efd[0];
    ls->oldefd = oldefd;

    __cxa_atexit(&close_stdio_fds, ls, NULL);

    if ((rc = pthread_attr_init(&pattr)) != 0)
        PANIC("can't init logcat writer thread attribute: %s", strerror(rc));
    if ((rc = pthread_attr_getschedpolicy(&pattr, &policy)) != 0)
        PANIC("can't get sched policy: %s", strerror(rc));
    if ((sparam.sched_priority = sched_get_priority_max(policy)) < 0)
        PANIC("can't get max priority: %s", strerror(errno));
    if ((rc = pthread_attr_setschedparam(&pattr, &sparam)) != 0)
        PANIC("can't set sched param: %s", strerror(rc));

    if ((rc = pthread_create(&pth, &pattr, stdio2logcat, ls)) != 0)
        PANIC("can't create logcat writer thread: %s", strerror(rc));
    if ((rc = pthread_detach(pth)) != 0)
        PANIC("can't detach logcat writer thread: %s", strerror(rc));

    if ((rc = pthread_attr_destroy(&pattr)) != 0)
        PANIC("can't destroy thread attr: %s", strerror(rc));
}
