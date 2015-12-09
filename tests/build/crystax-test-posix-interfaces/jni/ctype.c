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

#include <ctype.h>
#if __APPLE__ || __gnu_linux__
#include <xlocale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_CTYPE_H_XLOCALE_H_INCLUDED
#error Extended locales support not enabled in ctype.h
#endif
#endif

#ifdef __XSI_VISIBLE

#ifndef _toupper
#error _toupper not defined
#endif

#ifndef _tolower
#error _tolower not defined
#endif

#endif /* __XSI_VISIBLE */

void check_ctype_functions(locale_t l)
{
#ifdef __XSI_VISIBLE
    (void)_toupper(0);
    (void)_tolower(0);
#endif

#if __BSD_VISIBLE
    (void)digittoint(0);
    (void)digittoint_l(0, l);
#endif

    (void)isalnum(0);
    (void)isalnum_l(0, l);
    (void)isalpha(0);
    (void)isalpha_l(0, l);
#ifdef __XSI_VISIBLE
    (void)isascii(0);
#endif
    (void)isblank(0);
    (void)isblank_l(0, l);
    (void)iscntrl(0);
    (void)iscntrl_l(0, l);
    (void)isdigit(0);
    (void)isdigit_l(0, l);
    (void)isgraph(0);
    (void)isgraph_l(0, l);
    (void)islower(0);
    (void)islower_l(0, l);
    (void)isprint(0);
    (void)isprint_l(0, l);
    (void)ispunct(0);
    (void)ispunct_l(0, l);
    (void)isspace(0);
    (void)isspace_l(0, l);
    (void)isupper(0);
    (void)isupper_l(0, l);
    (void)isxdigit(0);
    (void)isxdigit_l(0, l);
#ifdef __XSI_VISIBLE
    (void)toascii(0);
#endif
    (void)tolower(0);
    (void)tolower_l(0, l);
    (void)toupper(0);
    (void)toupper_l(0, l);
}
