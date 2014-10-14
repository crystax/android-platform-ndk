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

#if __APPLE__ || __ANDROID__

/* Include all headers having extended locale support and check that xlocale functions from all headers available */
#include <ctype.h>
#include <inttypes.h>
#include <langinfo.h>
#include <monetary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#if __APPLE__
#include <xlocale.h>
#endif

#include "helper.h"

#define CHECK(type) type JOIN(xlocale2_check_type_, __LINE__)

CHECK(locale_t);
CHECK(struct lconv);

void xlocale2_check_functions(nl_item ni, locale_t l)
{
    /* ctype.h */
    (void)isalnum_l(0, l);
    (void)isdigit_l(0, l);
    (void)isxdigit_l(0, l);
    /* inttypes.h */
    (void)strtoimax_l("", (char**)1234, 10, l);
    /* langinfo.h */
    (void)nl_langinfo_l(ni, l);
    /* monetary.h */
    (void)strfmon_l((char*)1234, (size_t)0, l, "%n", 0.0);
    /* stdio.h */
    (void)printf_l(l, "%d", 0);
    /* stdlib.h */
    (void)strtol_l("", (char**)1234, 10, l);
    /* string.h */
    (void)strcoll_l("", "", l);
    /* time.h */
    (void)strftime_l((char*)1234, (size_t)0, "%s", (const struct tm *)1234, l);
    /* wchar.h */
    (void)wcstol_l(L"", (wchar_t**)1234, 10, l);
    /* wctype.h */
    (void)iswalnum_l((wint_t)0, l);
    (void)iswdigit_l((wint_t)0, l);
    (void)iswxdigit_l((wint_t)0, l);
}

#endif /* __APPLE__ || __ANDROID__ */
