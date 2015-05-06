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

#include <wchar.h>
#if __APPLE__
#include <xlocale.h>
#endif
#if __APPLE__ || __gnu_linux__
#include <stdarg.h> /* for va_start */
#endif
#if __gnu_linux__
#include <stdio.h> /* for FILE */
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_WCHAR_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in wchar.h
#endif
#endif

#include "gen/wchar.inc"

#include "helper.h"

#define CHECK(type) type JOIN(wchar_check_type_, __LINE__)

CHECK(FILE);
CHECK(locale_t);
CHECK(mbstate_t);
CHECK(size_t);
CHECK(va_list);
CHECK(wchar_t);
CHECK(wctype_t);
CHECK(wint_t);
CHECK(struct tm *);

void wchar_check_functions(FILE *f, wint_t i, wctype_t t, locale_t l, ...)
{
    va_list args;
    va_start(args, l);

    (void)btowc(0);
    (void)fgetwc(f);
    (void)fgetws((wchar_t*)1234, 0, f);
    (void)fputwc((wchar_t)0, f);
    (void)fputws((const wchar_t *)1234, f);
    (void)fwide(f, 0);
    (void)fwprintf(f, L"%d", 0);
    (void)fwscanf(f, L"%d", (int*)1234);
    (void)getwc(f);
    (void)getwchar();
    (void)iswalnum(i);
    (void)iswalpha(i);
    (void)iswcntrl(i);
    (void)iswctype(i, t);
    (void)iswdigit(i);
    (void)iswgraph(i);
    (void)iswlower(i);
    (void)iswprint(i);
    (void)iswpunct(i);
    (void)iswspace(i);
    (void)iswupper(i);
    (void)iswxdigit(i);
    (void)mbrlen((const char *)1234, (size_t)0, (mbstate_t*)1234);
    (void)mbrtowc((wchar_t*)1234, (const char *)1234, (size_t)0, (mbstate_t*)1234);
    (void)mbsinit((const mbstate_t *)1234);
    (void)mbsnrtowcs((wchar_t*)1234, (const char **)1234, (size_t)0, (size_t)0, (mbstate_t*)1234);
    (void)mbsrtowcs((wchar_t*)1234, (const char **)1234, (size_t)0, (mbstate_t*)1234);
#if !__APPLE__
    (void)open_wmemstream((wchar_t**)1234, (size_t*)1234);
#endif
    (void)putwc((wchar_t)0, f);
    (void)putwchar((wchar_t)0);
    (void)swprintf((wchar_t*)1234, (size_t)0, L"%d", 0);
    (void)swscanf((const wchar_t*)1234, L"%d", (int*)1234);
    (void)towlower(i);
    (void)towupper(i);
    (void)ungetwc(i, f);
    (void)vfwprintf(f, L"%d", args);
    (void)vfwscanf(f, L"%d", args);
    (void)vswprintf((wchar_t*)1234, (size_t)0, L"%d", args);
    (void)vswscanf((const wchar_t*)1234, L"%d", args);
    (void)vwprintf(L"%s", args);
    (void)vwscanf(L"%d", args);
#if !__APPLE__ || defined(__MAC_10_7)
    (void)wcpcpy((wchar_t*)1234, L"");
    (void)wcpncpy((wchar_t*)1234, L"", (size_t)0);
#endif
    (void)wcrtomb((char*)1234, (wchar_t)0, (mbstate_t*)1234);
#if !__APPLE__ || defined(__MAC_10_7)
    (void)wcscasecmp(L"", L"");
    (void)wcscasecmp_l(L"", L"", l);
#endif
    (void)wcscat((wchar_t*)1234, L"");
    (void)wcschr(L"", (wchar_t)0);
    (void)wcscmp(L"", L"");
    (void)wcscoll(L"", L"");
    (void)wcscoll_l(L"", L"", l);
    (void)wcscpy((wchar_t*)1234, L"");
    (void)wcscspn(L"", L"");
#if !__APPLE__ || defined(__MAC_10_7)
    (void)wcsdup(L"");
#endif
    (void)wcsftime((wchar_t*)1234, (size_t)0, L"%s", (const struct tm *)1234);
    (void)wcslen(L"");
#if !__APPLE__ || defined(__MAC_10_7)
    (void)wcsncasecmp(L"", L"", (size_t)0);
    (void)wcsncasecmp_l(L"", L"", (size_t)0, l);
#endif
    (void)wcsncat((wchar_t*)1234, L"", (size_t)0);
    (void)wcsncmp(L"", L"", (size_t)0);
    (void)wcsncpy((wchar_t*)1234, L"", (size_t)0);
#if !__APPLE__ || defined(__MAC_10_7)
    (void)wcsnlen(L"", (size_t)0);
#endif
    (void)wcsnrtombs((char*)1234, (const wchar_t **)1234, (size_t)0, (size_t)0, (mbstate_t*)1234);
    (void)wcspbrk(L"", L"");
    (void)wcsrchr(L"", (wchar_t)0);
    (void)wcsrtombs((char*)1234, (const wchar_t **)1234, (size_t)0, (mbstate_t*)1234);
    (void)wcsspn(L"", L"");
    (void)wcsstr(L"", L"");
    (void)wcstod(L"", (wchar_t**)1234);
    (void)wcstof(L"", (wchar_t**)1234);
    (void)wcstok((wchar_t*)1234, L"", (wchar_t**)1234);
    (void)wcstol(L"", (wchar_t**)1234, 10);
    (void)wcstold(L"", (wchar_t**)1234);
    (void)wcstoll(L"", (wchar_t**)1234, 10);
    (void)wcstoul(L"", (wchar_t**)1234, 10);
    (void)wcstoull(L"", (wchar_t**)1234, 10);
    (void)wcswidth(L"", (size_t)0);
    (void)wcsxfrm((wchar_t*)1234, L"", (size_t)0);
    (void)wcsxfrm_l((wchar_t*)1234, L"", (size_t)0, l);
    (void)wctob(i);
    (void)wctype("");
    (void)wcwidth((wchar_t)0);
    (void)wmemchr(L"", L'a', (size_t)0);
    (void)wmemcmp(L"", L"", (size_t)0);
    (void)wmemcpy((wchar_t*)1234, L"", (size_t)0);
    (void)wmemmove((wchar_t*)1234, L"", (size_t)0);
    (void)wmemset((wchar_t*)1234, L'a', (size_t)0);
    (void)wprintf(L"%d", 0);
    (void)wscanf(L"%d", (int*)1234);
}
