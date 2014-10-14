#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#if 1
#define CRYSTAX_DEBUG_SINK CRYSTAX_SINK_STDOUT
#else
#define CRYSTAX_DEBUG_SINK CRYSTAX_SINK_LOGCAT
#endif

#define CRYSTAX_SINK_STDOUT 0
#define CRYSTAX_SINK_LOGCAT 1

#ifndef CRYSTAX_DEBUG_SINK
#error  CRYSTAX_DEBUG_SINK not defined
#endif

#include <crystax/private.h>

#if CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_LOGCAT

static int (*func_android_log_vprint)(int, const char *, const char *, va_list) = NULL;

static int initialized = 0;

#define atomic_fetch(p) __sync_add_and_fetch(p, 0)

#define def_atomic_swap(type, suffix) \
    static type atomic_swap_ ## suffix ( type v, type *ptr) \
    { \
        type prev; \
        do { \
            prev = *ptr; \
        } while (__sync_val_compare_and_swap(ptr, prev, v) != prev); \
        return prev; \
    }

def_atomic_swap(int, i)
def_atomic_swap(void *, p)

static int __crystax_vlogcat(int prio, const char *tag, const char *fmt, va_list ap)
{
    if (atomic_fetch(&initialized) == 0)
    {
        void *pc;
        void *pf;

        pc = dlopen("liblog.so", RTLD_NOW);
        if (!pc) abort();

        pf = dlsym(pc, "__android_log_vprint");
        if (!pf) abort();

        atomic_swap_p(pf, (void*)&func_android_log_vprint);
        atomic_swap_i(1, &initialized);
    }
    return func_android_log_vprint(prio, tag, fmt, ap);
}
#endif /* CRYSTAX_SINK_LOGCAT */

const char *__crystax_log_short_file(const char *f)
{
    int const MAXLEN = 25;
    int flen = strlen(f);

    if (flen < MAXLEN)
        return f;

    return f + (flen - MAXLEN);
}

int __crystax_log(int prio, const char *tag,  const char *fmt, ...)
{
    int rc;
    va_list ap;

#if defined(CRYSTAX_DEBUG_SINK) && CRYSTAX_DEBUG_SINK != CRYSTAX_SINK_LOGCAT
    char buf[256];
    char *newfmt;
    int newfmtlen;

    newfmtlen = strlen(tag) + 2 + strlen(fmt) + 2;
    if (newfmtlen > (int)sizeof(buf))
    {
        newfmt = (char*)malloc(newfmtlen);
        if (!newfmt) abort();
    }
    else
        newfmt = buf;
    newfmt[0] = '\0';
    strcat(newfmt, tag);
    strcat(newfmt, ": ");
    strcat(newfmt, fmt);
    strcat(newfmt, "\n");
#endif

    va_start(ap, fmt);
#if defined(CRYSTAX_DEBUG_SINK) && CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_LOGCAT
    rc = __crystax_vlogcat(prio, tag, fmt, ap);
#elif defined(CRYSTAX_DEBUG_SINK) && CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_STDOUT
    rc = vfprintf(prio < CRYSTAX_LOGLEVEL_WARN ? stdout : stderr, newfmt, ap);
#else
#error Unknown debug sink
#endif

#if defined(CRYSTAX_DEBUG_SINK) && CRYSTAX_DEBUG_SINK != CRYSTAX_SINK_LOGCAT
    if (newfmt != buf)
        free(newfmt);
#endif

    va_end(ap);
    return rc;
}
