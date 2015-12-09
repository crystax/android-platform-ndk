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

#include <inttypes.h>
#if __APPLE__
#include <xlocale.h>
#endif
#if __gnu_linux__
#include <wchar.h> /* for wchar_t */
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_INTTYPES_H_XLOCALE_H_INCLUDED
#error Extended locales support not enabled in inttypes.h
#endif
#if !__LIBCRYSTAX_STDINT_H_INCLUDED
#error stdint.h not included in inttypes.h
#endif /* !__CRYSTAX_STDINT_H_INCLUDED */
#endif /* __ANDROID__ */

#include "gen/inttypes.inc"

#include "helper.h"

#define CHECK(type) type JOIN(var_inttypes_, __LINE__)

CHECK(intmax_t);
CHECK(uintmax_t);
CHECK(imaxdiv_t);
CHECK(wchar_t);
CHECK(locale_t);

void check_inttypes_functions(
#if __APPLE__ || __ANDROID__
        locale_t l
#endif
        )
{
    (void)imaxabs((intmax_t)0);
    (void)imaxdiv((intmax_t)0, (intmax_t)0);
    (void)strtoimax((const char *)1234, (char **)1234, 10);
    (void)strtoumax((const char *)1234, (char **)1234, 10);
    (void)wcstoimax((const wchar_t *)1234, (wchar_t **)1234, 10);
    (void)wcstoumax((const wchar_t *)1234, (wchar_t **)1234, 10);
#if __APPLE__ || __ANDROID__
    (void)strtoimax_l((const char *)1234, (char**)1234, 10, l);
    (void)strtoumax_l((const char *)1234, (char**)1234, 10, l);
    (void)wcstoimax_l((const wchar_t *)1234, (wchar_t **)1234, 10, l);
    (void)wcstoumax_l((const wchar_t *)1234, (wchar_t **)1234, 10, l);
#endif
}
