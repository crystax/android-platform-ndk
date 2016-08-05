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

#include <stdio.h>
#include <dlfcn.h>

#define CRYSTAX_LOG_SINK_STDOUT 0
#define CRYSTAX_LOG_SINK_LOGCAT 1

#define CRYSTAX_LOG_SINK CRYSTAX_LOG_SINK_LOGCAT

#ifndef CRYSTAX_LOG_SINK
#define CRYSTAX_LOG_SINK CRYSTAX_LOG_SINK_STDOUT
#endif

#ifndef CRYSTAX_LOG_SINK
#error  CRYSTAX_LOG_SINK not defined
#endif

#include <crystax/private.h>
#include <crystax/atomic.h>

#define __noinstrument __attribute__ ((no_instrument_function))

extern __noreturn void _exit(int);

__noinstrument
const char *__crystax_log_short_file(const char *f)
{
    int const MAXLEN = 25;
    const char *s;

    for (s = f; *s != '\0'; ++s);

    if ((s - f) < MAXLEN)
        return f;

    return s - MAXLEN;
}

__noinstrument
__noreturn static void __crystax_log_abort(int line)
{
    /* We can't call abort() function here since logging calls could be called from
     * the code on very early stage of loading library.
     * We rely on the fact that accesing low address, either debuggerd or kernel's
     * crash dump will show the fault address.
     */
    *(unsigned int*)((uintptr_t)line) = 0;
    _exit(line > 0 && line < 255 ? line : 255);
}

static int (*func_android_log_vprint)(int, const char *, const char *, va_list) = NULL;

__noinstrument
int __crystax_vlogcat(int prio, const char *tag, const char *fmt, va_list ap)
{
    if (__crystax_atomic_fetch(&func_android_log_vprint) == NULL)
    {
        void *pc;
        void *pf;

        pc = dlopen("liblog.so", RTLD_LOCAL | RTLD_NOW);
        if (!pc) __crystax_log_abort(__LINE__);

        pf = dlsym(pc, "__android_log_vprint");
        if (!pf) __crystax_log_abort(__LINE__);

        __crystax_atomic_swap(&func_android_log_vprint, pf);
    }

    return func_android_log_vprint(prio, tag, fmt, ap);
}

__noinstrument
int __crystax_logcat(int prio, const char *tag, const char *fmt, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt);
    rc = __crystax_vlogcat(prio, tag, fmt, ap);
    va_end(ap);

    return rc;
}

__noinstrument
int __crystax_vlogstd(int prio, const char *tag, const char *fmt, va_list ap)
{
    int rc;

    char *newfmtfmt = "%s: %s\n";
    rc = snprintf(NULL, 0, newfmtfmt, tag, fmt);
    if (rc < 0)
    {
        fprintf(stderr, "CRYSTAX_PANI: can't create new format string\n");
        __crystax_log_abort(__LINE__);
    }
    if (rc > 4096)
    {
        fprintf(stderr, "CRYSTAX_PANI: format string too long: \"%s\"\n", fmt);
        __crystax_log_abort(__LINE__);
    }

    char newfmt[rc + 1];
    rc = snprintf(newfmt, sizeof(newfmt), newfmtfmt, tag, fmt);
    if (rc < 0)
    {
        fprintf(stderr, "CRYSTAX_PANI: can't create new format string\n");
        __crystax_log_abort(__LINE__);
    }

    FILE *fp = prio < CRYSTAX_LOGLEVEL_WARN ? stdout : stderr;
    rc = vfprintf(fp, newfmt, ap);
    fflush(fp);

    return rc;
}

__noinstrument
int __crystax_logstd(int prio, const char *tag, const char *fmt, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt);
    rc = __crystax_vlogstd(prio, tag, fmt, ap);
    va_end(ap);

    return rc;
}

__noinstrument
int __crystax_vlog(int prio, const char *tag, const char *fmt, va_list ap)
{
#if CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_LOGCAT
    return __crystax_vlogcat(prio, tag, fmt, ap);
#elif CRYSTAX_LOG_SINK == CRYSTAX_LOG_SINK_STDOUT
    return __crystax_vlogstd(prio, tag, fmt, ap);
#else
#error Unknown log sink
#endif
}

__noinstrument
int __crystax_log(int prio, const char *tag,  const char *fmt, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt);
    rc = __crystax_vlog(prio, tag, fmt, ap);
    va_end(ap);

    return rc;
}

__noinstrument
void __cyg_profile_func_enter(void *fn, void *caller)
{
    __crystax_logcat(CRYSTAX_LOGLEVEL_INFO, "LIBCRYSTAX", "ENTER: fn=%p, caller=%p", fn, caller);
}

__noinstrument
void __cyg_profile_func_exit(void *fn, void *caller)
{
    __crystax_logcat(CRYSTAX_LOGLEVEL_INFO, "LIBCRYSTAX", "LEAVE: fn=%p, caller=%p", fn, caller);
}
