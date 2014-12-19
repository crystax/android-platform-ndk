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

#ifndef __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8
#define __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8

#include <crystax/id.h>

#if !defined(_POSIX_SOURCE)
#define _POSIX_SOURCE
#endif

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE < 200809
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 200809

#if defined(__POSIX_VISIBLE) && __POSIX_VISIBLE < 200809
#undef __POSIX_VISIBLE
#endif
#define __POSIX_VISIBLE 200809

#if defined(__XSI_VISIBLE) && __XSI_VISIBLE < 600
#undef __XSI_VISIBLE
#endif
#define __XSI_VISIBLE 600

#if defined(__ISO_C_VISIBLE) && __ISO_C_VISIBLE < 1999
#undef __ISO_C_VISIBLE
#endif
#define __ISO_C_VISIBLE 1999

#include <crystax/google/sys/cdefs.h>


#ifndef __has_extension
#define __has_extension     __has_feature
#endif
#ifndef __has_feature
#define __has_feature(x)    0
#endif
#ifndef __has_include
#define __has_include(x)    0
#endif
#ifndef __has_builtin
#define __has_builtin(x)    0
#endif

#ifdef __cplusplus
#define __nothrow throw()
#else
#define __nothrow
#endif

#ifdef __weak_alias
#undef __weak_alias
#endif
#define __weak_alias(sym, alias) \
    __asm__(".weak " #alias); \
    __asm__(".equ "  #alias ", " #sym);

#ifdef __weak_reference
#undef __weak_reference
#endif
#define __weak_reference(s, a) __weak_alias(s, a)

#ifdef __strong_reference
#undef __strong_reference
#endif
#define __strong_reference(sym,aliassym) \
    extern __typeof (sym) aliassym __attribute__ ((__alias__ (#sym)))

#ifdef __warn_references
#undef __warn_references
#endif
#define __warn_references(sym, msg)
/*
 * TODO: Enable this implementation. See https://tracker.crystax.net/issues/756 for details.
#define __warn_references(sym, msg) \
    __asm__(".section .gnu.warning." #sym "\n\t.ascii \"" msg "\"\n\t.text");
*/

#ifndef __always_inline
#define __always_inline __attribute__((__always_inline__))
#endif


#ifndef __GNUC_PREREQ__
#define __GNUC_PREREQ__(ma, mi) \
    (__GNUC__ > (ma) || __GNUC__ == (ma) && __GNUC_MINOR__ >= (mi))
#endif

#ifndef __LONG_LONG_SUPPORTED
#define __LONG_LONG_SUPPORTED
#endif

#ifndef _DECLARE_C99_LDBL_MATH
#define _DECLARE_C99_LDBL_MATH 1
#endif

#define __GNUCLIKE_ASM 3
#define __GNUCLIKE_MATH_BUILTIN_CONSTANTS
#define __GNUCLIKE___TYPEOF 1
#define __GNUCLIKE___OFFSETOF 1
#define __GNUCLIKE___SECTION 1

#ifndef asm
#define asm __asm
#endif

#endif /* __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8 */
