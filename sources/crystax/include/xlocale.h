/*
 * This library contains code from libc library of FreeBSD project which by-turn contains
 * code from other projects. To see specific authors and/or licenses, look into appropriate
 * source file. Here is license for those parts which are not derived from any other projects
 * but written by CrystaX.
 *
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

#ifndef _XLOCALE_H_
#define _XLOCALE_H_
#endif

#include <locale.h>

__BEGIN_DECLS

#ifndef __LIBCRYSTAX_XLOCALE__LOCALE_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__LOCALE_H_INCLUDED
#include <xlocale/_locale.h>
#endif

#ifdef _STRING_H_
#ifndef __LIBCRYSTAX_XLOCALE__STRING_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__STRING_H_INCLUDED
#include <xlocale/_string.h>
#endif
#endif

#ifdef _INTTYPES_H_
#ifndef __LIBCRYSTAX_XLOCALE__INTTYPES_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__INTTYPES_H_INCLUDED
#include <xlocale/_inttypes.h>
#endif
#endif

#ifdef _MONETARY_H_
#ifndef __LIBCRYSTAX_XLOCALE__MONETARY_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__MONETARY_H_INCLUDED
#include <xlocale/_monetary.h>
#endif
#endif

#ifdef _STDLIB_H_
#ifndef __LIBCRYSTAX_XLOCALE__STDLIB_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__STDLIB_H_INCLUDED
#include <xlocale/_stdlib.h>
#endif
#endif

#ifdef _TIME_H_
#ifndef __LIBCRYSTAX_XLOCALE__TIME_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__TIME_H_INCLUDED
#include <xlocale/_time.h>
#endif
#endif

#ifdef _LANGINFO_H_
#ifndef __LIBCRYSTAX_XLOCALE__LANGINFO_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__LANGINFO_H_INCLUDED
#include <xlocale/_langinfo.h>
#endif
#endif

#ifdef _CTYPE_H_
#ifndef __LIBCRYSTAX_XLOCALE__CTYPE_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__CTYPE_H_INCLUDED
#undef _XLOCALE_WCTYPES
#include <xlocale/_ctype.h>
#endif
#endif

#ifdef _WCTYPE_H_
#ifndef __LIBCRYSTAX_XLOCALE__WCTYPE_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__WCTYPE_H_INCLUDED
#define _XLOCALE_WCTYPES 1
#include <xlocale/_ctype.h>
#endif
#endif

#ifdef _STDIO_H_
#ifndef __LIBCRYSTAX_XLOCALE__STDIO_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__STDIO_H_INCLUDED
#include <xlocale/_stdio.h>
#endif
#endif

#ifdef _WCHAR_H_
#ifndef __LIBCRYSTAX_XLOCALE__WCHAR_H_INCLUDED
#define __LIBCRYSTAX_XLOCALE__WCHAR_H_INCLUDED
#include <xlocale/_wchar.h>
#endif
#endif

#ifndef __LIBCRYSTAX_XLOCALE_H_FUNCTIONS_DECLARED
#define __LIBCRYSTAX_XLOCALE_H_FUNCTIONS_DECLARED

struct lconv *localeconv_l(locale_t);

#endif

__END_DECLS
