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

#ifndef __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8
#define __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8

#include <crystax/id.h>
#include <crystax/google/sys/cdefs.h>

#if !defined(_POSIX_SOURCE)
#define _POSIX_SOURCE
#endif

#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809
#elif _POSIX_C_SOURCE < 200809
#undef  _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809
#endif

#if !defined(__POSIX_VISIBLE)
#define __POSIX_VISIBLE 200809
#elif __POSIX_VISIBLE < 200809
#undef  __POSIX_VISIBLE
#define __POSIX_VISIBLE 200809
#endif

#if !defined(__XSI_VISIBLE)
#define __XSI_VISIBLE 600
#elif __XSI_VISIBLE < 600
#undef  __XSI_VISIBLE
#define __XSI_VISIBLE 600
#endif

#if !defined(__ISO_C_VISIBLE)
#define __ISO_C_VISIBLE 1999
#elif __ISO_C_VISIBLE < 1999
#undef  __ISO_C_VISIBLE
#define __ISO_C_VISIBLE 1999
#endif

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

#define __nonnull(args) __attribute__((__nonnull__ args))

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

#ifdef __strong_alias
#undef __strong_alias
#endif
#define __strong_alias(alias, sym) \
    __asm__(".global " #alias); \
    __asm__(".equ "    #alias ", " #sym);

#ifdef __strong_reference
#undef __strong_reference
#endif
#define __strong_reference(s, a) __strong_alias(a, s)
/*
#define __strong_reference(sym,aliassym) \
    extern __typeof(sym) aliassym __attribute__ ((__alias__ (#sym)))
*/

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

#ifndef __errorattr
#ifdef __clang__
#define __errorattr(msg)
#else
#define __errorattr(msg) __attribute__((__error__(msg)))
#endif
#endif

#ifndef __warnattr
#ifdef __clang__
#define __warnattr(msg)
#else
#define __warnattr(msg) __attribute__((__warning__(msg)))
#endif
#endif

#ifndef __errordecl
#define __errordecl(name, msg) extern void name(void) __errorattr(msg)
#endif

#ifndef __wur
#define __wur __attribute__((__warn_unused_result__))
#endif

#ifndef __GNUC_PREREQ__
#define __GNUC_PREREQ__(ma, mi) \
    (__GNUC__ > (ma) || __GNUC__ == (ma) && __GNUC_MINOR__ >= (mi))
#endif

#if __GNUC_PREREQ__(4, 0)
#define __hidden   __attribute__((__visibility__("hidden")))
#define __exported __attribute__((__visibility__("default")))
#else
#define __hidden
#define __exported
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

#ifdef _BIONIC_NOT_BEFORE_21
#undef _BIONIC_NOT_BEFORE_21
#endif
#define _BIONIC_NOT_BEFORE_21(x) x

#ifndef __LIBC_HIDDEN__
#define __LIBC_HIDDEN__ __hidden
#endif

#ifndef __LIBC64_HIDDEN__
#define __LIBC64_HIDDEN__ __LIBC_HIDDEN__
#endif

#ifndef __LIBC_ABI_PUBLIC__
#define __LIBC_ABI_PUBLIC__ __exported
#endif

#ifndef _DIAGASSERT
#define _DIAGASSERT(e) ((e) ? (void) 0 : __assert2(__FILE__, __LINE__, __func__, #e))
#endif

#ifndef __type_fit
#define __type_fit(t, a) (0 == 0)
#endif

#define __USE_GNU 1
#define __USE_BSD 1
#define __BSD_VISIBLE 1

#endif /* __CRYSTAX_SYS_CDEFS_H_649B3B19BE21490D983FE57C973D2BA8 */
