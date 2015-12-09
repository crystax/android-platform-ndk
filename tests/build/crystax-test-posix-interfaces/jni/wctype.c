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

#include <wctype.h>
#if __APPLE__
#include <xlocale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_WCTYPE_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in wctype.h
#endif
#endif

#if !defined(WEOF)
#error WEOF not defined
#endif

#include "helper.h"

#define CHECK(type) type JOIN(wctype_check_type_, __LINE__)

CHECK(wint_t);
CHECK(wctrans_t);
CHECK(wctype_t);
CHECK(locale_t);

void wctype_check_functions(wint_t i, wctype_t t, wctrans_t tr, locale_t l)
{
    (void)iswalnum(i);
    (void)iswalnum_l(i, l);
    (void)iswalpha(i);
    (void)iswalpha_l(i, l);
    (void)iswblank(i);
    (void)iswblank_l(i, l);
    (void)iswcntrl(i);
    (void)iswcntrl_l(i, l);
    (void)iswctype(i, t);
    (void)iswctype_l(i, t, l);
    (void)iswdigit(i);
    (void)iswdigit_l(i, l);
    (void)iswgraph(i);
    (void)iswgraph_l(i, l);
    (void)iswlower(i);
    (void)iswlower_l(i, l);
    (void)iswprint(i);
    (void)iswprint_l(i, l);
    (void)iswpunct(i);
    (void)iswpunct_l(i, l);
    (void)iswspace(i);
    (void)iswspace_l(i, l);
    (void)iswupper(i);
    (void)iswupper_l(i, l);
    (void)iswxdigit(i);
    (void)iswxdigit_l(i, l);
    (void)towctrans(i, tr);
    (void)towctrans_l(i, tr, l);
    (void)towlower(i);
    (void)towlower_l(i, l);
    (void)towupper(i);
    (void)towupper_l(i, l);
    (void)wctrans((const char *)1234);
    (void)wctrans_l((const char *)1234, l);
    (void)wctype((const char *)1234);
    (void)wctype_l((const char *)1234, l);
}
