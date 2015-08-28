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

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>

#define CRYSTAX_LOG_SINK_STDOUT 0
#define CRYSTAX_LOG_SINK_LOGCAT 1

#ifndef CRYSTAX_LOG_SINK
#define CRYSTAX_LOG_SINK CRYSTAX_LOG_SINK_STDOUT
#endif

#ifndef CRYSTAX_LOG_SINK
#error  CRYSTAX_LOG_SINK not defined
#endif

#include <crystax/private.h>
#include <crystax/atomic.h>

#if CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_LOGCAT

static int (*func_android_log_vprint)(int, const char *, const char *, va_list) = NULL;

static int initialized = 0;

static int __crystax_vlogcat(int prio, const char *tag, const char *fmt, va_list ap)
{
    if (__crystax_atomic_fetch(&initialized) == 0)
    {
        void *pc;
        void *pf;

        pc = dlopen("liblog.so", RTLD_NOW);
        if (!pc) abort();

        pf = dlsym(pc, "__android_log_vprint");
        if (!pf) abort();

        __crystax_atomic_swap(&func_android_log_vprint, pf);
        __crystax_atomic_swap(&initialized, 1);
    }
    return func_android_log_vprint(prio, tag, fmt, ap);
}
#endif /* CRYSTAX_LOG_SINK_LOGCAT */

const char *__crystax_log_short_file(const char *f)
{
    int const MAXLEN = 25;
    const char *s;

    for (s = f; *s != '\0'; ++s);

    if ((s - f) < MAXLEN)
        return f;

    return s - MAXLEN;
}

int __crystax_log(int prio, const char *tag,  const char *fmt, ...)
{
    int rc;
    va_list ap;
    int serrno;

    serrno = errno;

#if CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_STDOUT
    char *newfmtfmt = "%s: %s\n";
    rc = snprintf(NULL, 0, newfmtfmt, tag, fmt);
    if (rc < 0)
    {
        fprintf(stderr, "CRYSTAX_PANI: can't create new format string\n");
        abort();
    }
    if (rc > 4096)
    {
        fprintf(stderr, "CRYSTAX_PANI: format string too long: \"%s\"\n", fmt);
        abort();
    }
    char newfmt[rc + 1];
    rc = snprintf(newfmt, sizeof(newfmt), newfmtfmt, tag, fmt);
    if (rc < 0)
    {
        fprintf(stderr, "CRYSTAX_PANI: can't create new format string\n");
        abort();
    }
#endif

    va_start(ap, fmt);
#if CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_LOGCAT
    rc = __crystax_vlogcat(prio, tag, fmt, ap);
#elif CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_STDOUT
    rc = vfprintf(prio < CRYSTAX_LOGLEVEL_WARN ? stdout : stderr, newfmt, ap);
#else
#error Unknown log sink
#endif

    va_end(ap);

    errno = serrno;

    return rc;
}
