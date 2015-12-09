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

#include <stdlib.h>
#if __APPLE__
#include <sys/wait.h>
#endif
#if __APPLE__
#include <xlocale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_STDLIB_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in stdlib.h
#endif
#endif

#include "gen/stdlib.inc"

#include "helper.h"

#define CHECK(type) type JOIN(stdlib_check_type_, __LINE__);

CHECK(div_t);
CHECK(ldiv_t);
CHECK(lldiv_t);
CHECK(size_t);
CHECK(wchar_t);
#if __APPLE__ || __ANDROID__
CHECK(locale_t);
#endif

#undef CHECK

void * check_stdlib_functions(
#if __APPLE__ || __ANDROID__
        locale_t l
#endif
        )
{
    void *memptr;

    (void)_Exit(0);
#if __XSI_VISIBLE
    (void)a64l("1");
#endif
    (void)abort();
    (void)abs(1);
    (void)atexit((void (*)(void))1234);
    (void)atof("1");
#if __APPLE__ || __ANDROID__
    (void)atof_l("1", l);
#endif
    (void)atoi("1");
#if __APPLE__ || __ANDROID__
    (void)atoi_l("1", l);
#endif
    (void)atol("1");
#if __APPLE__ || __ANDROID__
    (void)atol_l("1", l);
#endif
    (void)atoll("1");
#if __APPLE__ || __ANDROID__
    (void)atoll_l("1", l);
#endif
    (void)bsearch((const void*)1234, (const void*)1234, 1, 1, (int (*)(const void *, const void *))1234);
    (void)calloc(1, 2);
    (void)div(1, 2);
#if __XSI_VISIBLE
    (void)drand48();
    (void)erand48(NULL);
#endif
    (void)exit(0);
    (void)free(NULL);
    (void)getenv("VARNAME");
#if __XSI_VISIBLE
    (void)getsubopt((char **)NULL, (char * const *)NULL, (char **)NULL);
    (void)grantpt(1);
    (void)initstate((unsigned)1, (char*)NULL, (size_t)1);
    (void)jrand48(NULL);
    (void)l64a(0);
#endif
    (void)labs((long)1);
#if __XSI_VISIBLE
    (void)lcong48(NULL);
#endif
    (void)ldiv((long)1, (long)2);
    (void)llabs((long long)1);
    (void)lldiv((long long)1, (long long)2);
#if __XSI_VISIBLE
    (void)lrand48();
#endif
    (void)malloc((size_t)2);
    (void)mblen((const char *)1234, (size_t)1);
#if __APPLE__ || __ANDROID__
    (void)mblen_l((const char *)1234, (size_t)1, l);
#endif
    (void)mbstowcs((wchar_t*)1234, (const char *)1234, (size_t)0);
#if __APPLE__ || __ANDROID__
    (void)mbstowcs_l((wchar_t*)1234, (const char *)1234, (size_t)0, l);
#endif
    (void)mbtowc((wchar_t*)1234, (const char *)1234, (size_t)0);
#if __APPLE__ || __ANDROID__
    (void)mbtowc_l((wchar_t*)1234, (const char *)1234, (size_t)0, l);
#endif
#if __XSI_VISIBLE
    (void)mkstemp((char*)NULL);
    (void)mrand48();
    (void)nrand48(NULL);
#endif
    (void)posix_memalign(&memptr, (size_t)0, (size_t)0);
#if __XSI_VISIBLE
    (void)posix_openpt(0);
    (void)ptsname(0);
    (void)putenv("VARNAME");
#endif
    (void)qsort((void*)1234, (size_t)0, (size_t)0, (int (*)(const void *, const void *))1234);
    (void)rand();
    (void)rand_r(NULL);
#if __XSI_VISIBLE
    (void)random();
#endif
    memptr = realloc(NULL, 0);
#if __XSI_VISIBLE
    (void)realpath((const char *)NULL, (char*)NULL);
    (void)seed48(NULL);
#endif
    (void)setenv("NAME", "VALUE", 1);
#if __XSI_VISIBLE
    (void)setkey((const char *)NULL);
    (void)setstate((char *)NULL);
#endif
    (void)srand((unsigned)0);
#if __XSI_VISIBLE
    (void)srand48((long)0);
    (void)srandom((unsigned)0);
#endif
    (void)strtod((const char *)1234, (char **)1234);
    (void)strtof((const char *)1234, (char **)1234);
    (void)strtol((const char *)1234, (char **)1234, 10);
    (void)strtold((const char *)1234, (char **)1234);
    (void)strtoll((const char *)1234, (char **)1234, 10);
    (void)strtoul((const char *)1234, (char **)1234, 10);
    (void)strtoull((const char *)1234, (char **)1234, 10);
#if __APPLE__ || __ANDROID__
    (void)strtod_l((const char*)1234, (char **)1234, l);
    (void)strtof_l((const char *)1234, (char **)1234, l);
    (void)strtol_l((const char *)1234, (char **)1234, 10, l);
    (void)strtold_l((const char *)1234, (char **)1234, l);
    (void)strtoll_l((const char *)1234, (char **)1234, 10, l);
    (void)strtoul_l((const char *)1234, (char **)1234, 10, l);
    (void)strtoull_l((const char *)1234, (char **)1234, 10, l);
#endif
    (void)system("bin");
#if __XSI_VISIBLE
    (void)unlockpt(0);
#endif
    (void)unsetenv("VARNAME");
    (void)wcstombs((char *)1234, (const wchar_t *)1234, (size_t)0);
    (void)wctomb((char*)1234, (wchar_t)0);
#if __APPLE__ || __ANDROID__
    (void)wcstombs_l((char*)1234, (const wchar_t *)1234, (size_t)0, l);
    (void)wctomb_l((char*)1234, (wchar_t)0, l);
#endif

    return memptr;
}
