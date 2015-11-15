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

#ifndef __CRYSTAX_SYS_TYPES_H_D348E9D46FFA4B97A56C4EEFAB6B3F76
#define __CRYSTAX_SYS_TYPES_H_D348E9D46FFA4B97A56C4EEFAB6B3F76

#include <crystax/id.h>

#include <sys/cdefs.h>
#include <sys/_types.h>

#include <stdint.h>

#ifndef _OFF_T_DECLARED
typedef __off_t off_t;
#define _OFF_T_DECLARED
#endif

#ifndef _LOFF_T_DECLARED
typedef __loff_t loff_t;
#define _LOFF_T_DECLARED
#endif

#ifndef _OFF64_T_DECLARED
typedef __off64_t off64_t;
#define _OFF64_T_DECLARED
#endif

#ifndef _REGISTER_T_DECLARED
typedef __register_t   register_t;
#define _REGISTER_T_DECLARED
#endif

#ifndef _UREGISTER_T_DECLARED
typedef __u_register_t u_register_t;
#define _UREGISTER_T_DECLARED
#endif

#ifndef _PID_T_DECLARED
typedef __pid_t pid_t;
#define _PID_T_DECLARED
#endif

#ifndef _UID_T_DECLARED
typedef __uid_t uid_t;
#define _UID_T_DECLARED
#endif

#ifndef _GID_T_DECLARED
typedef __gid_t gid_t;
#define _GID_T_DECLARED
#endif

#ifndef _ID_T_DECLARED
typedef __id_t id_t;
#define _ID_T_DECLARED
#endif

#ifndef _CLOCK_T_DECLARED
typedef __clock_t clock_t;
#define _CLOCK_T_DECLARED
#endif

#ifndef _CLOCKID_T_DECLARED
typedef __clockid_t clockid_t;
#define _CLOCKID_T_DECLARED
#endif

#ifndef _TIME_T_DECLARED
typedef __time_t time_t;
#define _TIME_T_DECLARED
#endif

#ifndef _TIMER_T_DECLARED
typedef __timer_t timer_t;
#define _TIMER_T_DECLARED
#endif

#ifndef _USECONDS_T_DECLARED
typedef __useconds_t useconds_t;
#define _USECONDS_T_DECLARED
#endif

#ifndef _SUSECONDS_T_DECLARED
typedef __suseconds_t suseconds_t;
#define _SUSECONDS_T_DECLARED
#endif

#ifndef _DEV_T_DECLARED
typedef __dev_t dev_t;
#define _DEV_T_DECLARED
#endif

#ifndef _MODE_T_DECLARED
typedef __mode_t mode_t;
#define _MODE_T_DECLARED
#endif

#ifndef _SOCKLEN_T_DECLARED
typedef __socklen_t socklen_t;
#define _SOCKLEN_T_DECLARED
#endif

#ifndef _IN_PORT_T_DECLARED
typedef __in_port_t in_port_t;
#define _IN_PORT_T_DECLARED
#endif

#ifndef _LOCALE_T_DEFINED
#define _LOCALE_T_DEFINED
typedef __locale_t locale_t;
#endif

#ifndef _PTHREAD_T_DECLARED
typedef __pthread_t pthread_t;
#define _PTHREAD_T_DECLARED
#endif

#ifndef _PTHREAD_ATTR_T_DECLARED
typedef __pthread_attr_t pthread_attr_t;
#define _PTHREAD_ATTR_T_DECLARED
#endif

#ifndef _PTHREAD_MUTEX_T_DECLARED
typedef __pthread_mutex_t pthread_mutex_t;
#define _PTHREAD_MUTEX_T_DECLARED
#endif

#ifndef _PTHREAD_COND_T_DECLARED
typedef __pthread_cond_t pthread_cond_t;
#define _PTHREAD_COND_T_DECLARED
#endif

#if __ANDROID_API__ < 21
typedef __kernel_blkcnt_t  blkcnt_t;
typedef __kernel_blksize_t blksize_t;
#else
typedef unsigned long      blkcnt_t;
typedef unsigned long      blksize_t;
#endif

typedef __kernel_ino_t     ino_t;
typedef __kernel_key_t     key_t;

#if __ANDROID_API__ < 21
typedef __kernel_nlink_t   nlink_t;
#else
typedef __uint32_t         nlink_t;
#endif

#ifdef __BSD_VISIBLE
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;

typedef __uint32_t       u_int32_t;
typedef __uint16_t       u_int16_t;
typedef __uint8_t        u_int8_t;
typedef __uint64_t       u_int64_t;

typedef __int64_t        quad_t;
typedef __uint64_t       u_quad_t;
typedef quad_t *         qaddr_t;
#endif

typedef char *       caddr_t;
typedef const char * c_caddr_t;

typedef unsigned long fsblkcnt_t;
typedef unsigned long fsfilcnt_t;

#endif /* __CRYSTAX_SYS_TYPES_H_D348E9D46FFA4B97A56C4EEFAB6B3F76 */
