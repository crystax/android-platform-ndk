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

#ifndef __CRYSTAX_FREEBSD_H_0E6882890F9949FDA2CAC835E74E8C53
#define __CRYSTAX_FREEBSD_H_0E6882890F9949FDA2CAC835E74E8C53

#include <crystax/ctassert.h>
#include <sys/cdefs.h>

#ifdef _KERNEL
#undef _KERNEL
#endif

#ifndef __hidden
#define __hidden __attribute__((__visibility__("hidden")))
#endif

#ifndef __exported
#define __exported __attribute__((__visibility__("default")))
#endif

#ifndef __pure
#define __pure __attribute__((__pure__))
#endif

#ifndef __pure2
#define __pure2 __attribute__((__const__))
#endif

#ifndef __dead2
#define __dead2 __attribute__((__noreturn__))
#endif

#ifndef __malloc_like
#define __malloc_like __attribute__((__malloc__))
#endif

#ifndef __printf0like
#define __printf0like(fmtarg, firstvararg) __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif

#ifndef __printflike
#define __printflike(fmtarg, firstvararg) __attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif

#ifndef __scanflike
#define __scanflike(fmtarg, firstvararg)  __attribute__((__format__ (__scanf__, fmtarg, firstvararg)))
#endif

#ifndef __strfmonlike
#define __strfmonlike(fmtarg, firstvararg) __attribute__((__format__ (__strfmon__, fmtarg, firstvararg)))
#endif

#ifndef __strftimelike
#define __strftimelike(fmtarg, firstvararg) __attribute__((__format__ (__strftime__, fmtarg, firstvararg)))
#endif

#ifndef __format_arg
#define __format_arg(fmtarg) __attribute__((__format_arg__ (fmtarg)))
#endif

#ifndef __noinline
#define __noinline __attribute__ ((__noinline__))
#endif

#ifdef __FBSDID
#undef __FBSDID
#endif
#define __FBSDID(x) /* nothing */

#ifndef __DECONST
#define __DECONST(type, var) ((type)(__uintptr_t)(const void *)(var))
#endif

#ifndef __DEVOLATILE
#define __DEVOLATILE(type, var) ((type)(__uintptr_t)(volatile void *)(var))
#endif

#ifdef __NO_TLS
#undef __NO_TLS
#endif

#if !defined(__compiler_membar)
#define __compiler_membar() __asm __volatile(" " : : : "memory")
#endif

#ifdef __cplusplus
extern "C"
#endif
int __crystax_isthreaded();
#define __isthreaded __crystax_isthreaded()

#if defined(__clang__)
#if !__has_extension(c_thread_local)
#if __has_extension(cxx_thread_local)
#define _Thread_local thread_local
#else
#define _Thread_local __thread
#endif
#endif
#else
#define _Thread_local __thread
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#define _Noreturn [[noreturn]]
#else
#define _Noreturn __dead2
#endif

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef powerof2
#define powerof2(x) ((((x)-1)&(x))==0)
#endif

#define __RUNETYPE_INTERNAL 1

#endif /* __CRYSTAX_FREEBSD_H_0E6882890F9949FDA2CAC835E74E8C53 */
