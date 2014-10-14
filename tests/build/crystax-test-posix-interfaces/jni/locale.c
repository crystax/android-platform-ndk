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

#include <locale.h>
#if __APPLE__
#include <xlocale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_LOCALE_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in locale.h
#endif
#endif

#include "gen/locale.inc"

#include "helper.h"

#define CHECK(type) type JOIN(locale_check_type_, __LINE__)

CHECK(locale_t);

/* Check if mandatory fields are present */
struct lconv lcv = {
    .currency_symbol    = NULL,
    .decimal_point      = NULL,
    .frac_digits        = '\0',
    .grouping           = NULL,
    .int_curr_symbol    = NULL,
    .int_frac_digits    = '\0',
    .int_n_cs_precedes  = '\0',
    .int_n_sep_by_space = '\0',
    .int_n_sign_posn    = '\0',
    .int_p_cs_precedes  = '\0',
    .int_p_sep_by_space = '\0',
    .int_p_sign_posn    = '\0',
    .mon_decimal_point  = NULL,
    .mon_grouping       = NULL,
    .mon_thousands_sep  = NULL,
    .negative_sign      = NULL,
    .n_cs_precedes      = '\0',
    .n_sep_by_space     = '\0',
    .n_sign_posn        = '\0',
    .positive_sign      = NULL,
    .p_cs_precedes      = '\0',
    .p_sep_by_space     = '\0',
    .p_sign_posn        = '\0',
    .thousands_sep      = NULL,
};

void locale_check_functions(locale_t l)
{
    (void)duplocale(l);
    (void)freelocale(l);
    (void)localeconv();
    (void)newlocale(0, "C", l);
    (void)setlocale(0, "C");
    (void)uselocale(l);
}
