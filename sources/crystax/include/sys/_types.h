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

#ifndef __CRYSTAX_SYS__TYPES_H_B94AFA76630A41EC924D80883BAB5389
#define __CRYSTAX_SYS__TYPES_H_B94AFA76630A41EC924D80883BAB5389

#include <crystax/id.h>

#include <machine/_types.h>

#ifndef _SIZE_T_DECLARED
typedef __size_t  size_t;
#define _SIZE_T_DECLARED
#endif

#ifndef _SSIZE_T_DECLARED
typedef __ssize_t ssize_t;
#define _SSIZE_T_DECLARED
#endif

#if !defined(__LP64__)
typedef __kernel_off_t  __off_t;
typedef __kernel_loff_t __loff_t;
#else
typedef __kernel_off_t  __off_t;
typedef __kernel_loff_t __loff_t;
#endif
typedef __loff_t __off64_t;

typedef struct _xlocale *__locale_t;

/*
 * mbstate_t is an opaque object to keep conversion state during multibyte
 * stream conversions.
 */
typedef union {
    char        __mbstate8[128];
    __int64_t   _mbstateL;  /* for alignment */
} __mbstate_t;

#if !__LP64__
typedef __uint32_t __dev_t;
#else
typedef __uint64_t __dev_t;
#endif

typedef __uint16_t __mode_t;

typedef __kernel_pid_t   __pid_t;
typedef __kernel_uid32_t __uid_t;
typedef __kernel_gid32_t __gid_t;

typedef __kernel_clock_t   __clock_t;
typedef __kernel_clockid_t __clockid_t;
typedef __kernel_time_t    __time_t;

typedef void *__timer_t;

typedef __uint32_t           __useconds_t;
typedef __kernel_suseconds_t __suseconds_t;

typedef int __nl_item;

typedef __uint16_t __in_port_t;

typedef __uint16_t __sa_family_t;

typedef long __pthread_t;

typedef struct {
    int volatile value;
#ifdef __LP64__
    char __reserved[36];
#endif
} __pthread_mutex_t;

#endif /* __CRYSTAX_SYS__TYPES_H_B94AFA76630A41EC924D80883BAB5389 */
