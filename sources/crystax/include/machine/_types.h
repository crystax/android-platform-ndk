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

#ifndef __CRYSTAX_INCLUDE_MACHINE_TYPES_H_AEE543128E8C4EA492D3235B11B6890A
#define __CRYSTAX_INCLUDE_MACHINE_TYPES_H_AEE543128E8C4EA492D3235B11B6890A

#include <crystax/id.h>
#include <crystax/ctassert.h>

#include <android/api-level.h>

#include <asm/types.h>
#include <linux/types.h>
#include <linux/posix_types.h>
#if __ANDROID_API__ < 21
#include <machine/kernel.h>
#endif

#include <machine/_limits.h>

typedef signed char        __int8_t;
typedef unsigned char      __uint8_t;
typedef short              __int16_t;
typedef unsigned short     __uint16_t;
typedef int                __int32_t;
typedef unsigned int       __uint32_t;
#if !__LP64__
typedef long long          __int64_t;
typedef unsigned long long __uint64_t;
#else
typedef long               __int64_t;
typedef unsigned long      __uint64_t;
#endif

__CRYSTAX_STATIC_ASSERT(sizeof(__int8_t)   == 1, "__int8_t must be 1-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint8_t)  == 1, "__uint8_t must be 1-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int16_t)  == 2, "__int16_t must be 2-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint16_t) == 2, "__uint16_t must be 2-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int32_t)  == 4, "__int32_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint32_t) == 4, "__uint32_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int64_t)  == 8, "__int64_t must be 8-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint64_t) == 8, "__uint64_t must be 8-byte long");

typedef __int8_t   __int_least8_t;
typedef __uint8_t  __uint_least8_t;
typedef __int16_t  __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t  __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t  __int_least64_t;
typedef __uint64_t __uint_least64_t;

__CRYSTAX_STATIC_ASSERT(sizeof(__int_least8_t)   >= 1, "__int_least8_t must be at least 1-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_least8_t)  >= 1, "__uint_least8_t must be at least 1-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_least16_t)  >= 2, "__int_least16_t must be at least 2-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_least16_t) >= 2, "__uint_least16_t must be at least 2-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_least32_t)  >= 4, "__int_least32_t must be at least 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_least32_t) >= 4, "__uint_least32_t must be at least 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_least64_t)  >= 8, "__int_least64_t must be at least 8-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_least64_t) >= 8, "__uint_least64_t must be at least 8-byte long");

typedef __int32_t  __int_fast8_t;
typedef __uint32_t __uint_fast8_t;
typedef __int32_t  __int_fast16_t;
typedef __uint32_t __uint_fast16_t;
typedef __int32_t  __int_fast32_t;
typedef __uint32_t __uint_fast32_t;
typedef __int64_t  __int_fast64_t;
typedef __uint64_t __uint_fast64_t;

__CRYSTAX_STATIC_ASSERT(sizeof(__int_fast8_t)   == 4, "__int_fast8_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_fast8_t)  == 4, "__uint_fast8_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_fast16_t)  == 4, "__int_fast16_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_fast16_t) == 4, "__uint_fast16_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_fast32_t)  == 4, "__int_fast32_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_fast32_t) == 4, "__uint_fast32_t must be 4-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__int_fast64_t)  == 8, "__int_fast64_t must be 8-byte long");
__CRYSTAX_STATIC_ASSERT(sizeof(__uint_fast64_t) == 8, "__uint_fast64_t must be 8-byte long");

#if !__LP64__
typedef __int32_t  __intptr_t;
typedef __uint32_t __uintptr_t;
#else
typedef __int64_t  __intptr_t;
typedef __uint64_t __uintptr_t;
#endif

#if defined(__INTMAX_TYPE__)
typedef __INTMAX_TYPE__ __intmax_t;
#else
typedef __int64_t       __intmax_t;
#endif
__CRYSTAX_STATIC_ASSERT(sizeof(__intmax_t) == 8, "__intmax_t must be 8-byte long");

#if defined(__UINTMAX_TYPE__)
typedef __UINTMAX_TYPE__ __uintmax_t;
#else
typedef __uint64_t       __uintmax_t;
#endif
__CRYSTAX_STATIC_ASSERT(sizeof(__uintmax_t) == 8, "__uintmax_t must be 8-byte long");

#if defined(__SIZE_TYPE__)
typedef __SIZE_TYPE__ __size_t;
#else
#if __LP64__
typedef __uint64_t    __size_t;
#else
typedef __uint32_t    __size_t;
#endif
#endif

#if __LP64__
__CRYSTAX_STATIC_ASSERT(sizeof(__size_t) == 8, "__size_t must be 8-byte long on 64-bit platforms");
#else
__CRYSTAX_STATIC_ASSERT(sizeof(__size_t) == 4, "__size_t must be 4-byte long on 32-bit platforms");
#endif

#if __LP64__
typedef __int64_t  __ssize_t;
#else
typedef __int32_t  __ssize_t;
#endif

#if !__LP64__
typedef __int32_t  __register_t;
typedef __uint32_t __u_register_t;
#else
typedef __int64_t  __register_t;
typedef __uint64_t __u_register_t;
#endif

/*
 * rune_t is declared to be an ``int'' instead of the more natural
 * ``unsigned long'' or ``long''.  Two things are happening here.  It is not
 * unsigned so that EOF (-1) can be naturally assigned to it and used.  Also,
 * it looks like 10646 will be a 31 bit standard.  This means that if your
 * ints cannot hold 32 bits, you will be in trouble.  The reason an int was
 * chosen over a long is that the is*() and to*() routines take ints (says
 * ANSI C), but they use __ct_rune_t instead of int.
 *
 * NOTE: rune_t is not covered by ANSI nor other standards, and should not
 * be instantiated outside of lib/libc/locale.  Use wchar_t.  wint_t and
 * rune_t must be the same type.  Also, wint_t should be able to hold all
 * members of the largest character set plus one extra value (WEOF), and
 * must be at least 16 bits.
 */
typedef int         __ct_rune_t; /* arg type for ctype funcs */
typedef __ct_rune_t __rune_t;    /* rune_t (see above) */

typedef __ct_rune_t __wint_t;

#ifdef __WINT_TYPE__
/* must be the same size */
__CRYSTAX_STATIC_ASSERT(sizeof(__WINT_TYPE__) == sizeof(__ct_rune_t), "size of __WINT_TYPE__ and __wint_t must be the same");
/* and have the same sign and be comparable without warnings */
/* TODO: figure out how to make __WINT_TYPE__ signed in GCC/Clang */
/*__CRYSTAX_STATIC_ASSERT((__WINT_TYPE__)(-1) == (__wint_t)(-1), "__WINT_TYPE__ and __wint_t must be both signed or both unsigned");*/
#endif

#ifdef __WCHAR_TYPE__
typedef __WCHAR_TYPE__ __wchar_t;
#else
typedef __wint_t __wchar_t;
#endif
__CRYSTAX_STATIC_ASSERT(sizeof(__wint_t) == sizeof(__wchar_t), "size of __wchar_t must be the same as of __wint_t");

/* This type needed in newest FreeBSD sources */
typedef __wchar_t ___wchar_t;

#if __ARM_EABI__ || __aarch64__
#define __WCHAR_MIN 0
#define __WCHAR_MAX __UINT_MAX
#else
#define __WCHAR_MIN __INT_MIN
#define __WCHAR_MAX __INT_MAX
#endif

typedef float  __float_t;
typedef double __double_t;

typedef __builtin_va_list __va_list;

#if __ANDROID_API__ < 21
typedef __kernel_id_t __id_t;
#else
typedef __uint32_t    __id_t;
#endif
__CRYSTAX_STATIC_ASSERT(sizeof(__id_t) == 4, "__id_t must be 4-byte long");

typedef __uint32_t __socklen_t;

#endif /* __CRYSTAX_INCLUDE_MACHINE_TYPES_H_AEE543128E8C4EA492D3235B11B6890A */
