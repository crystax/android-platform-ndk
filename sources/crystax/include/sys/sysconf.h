/*
 * This library contains code from libc library of FreeBSD project which by-turn contains
 * code from other projects. To see specific authors and/or licenses, look into appropriate
 * source file. Here is license for those parts which are not derived from any other projects
 * but written by CrystaX .NET.
 *
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

#ifndef __CRYSTAX_INCLUDE_SYS_SYSCONF_H_7D99640560394B7CB625C2C31C7DF312
#define __CRYSTAX_INCLUDE_SYS_SYSCONF_H_7D99640560394B7CB625C2C31C7DF312

#include <crystax/id.h>
#include <crystax/google/sys/sysconf.h>

#define __CRYSTAX_SC_BASE 0x80000000

#define _SC_2_PBS                      ( __CRYSTAX_SC_BASE | 0x0001 )
#define _SC_2_PBS_ACCOUNTING           ( __CRYSTAX_SC_BASE | 0x0002 )
#define _SC_2_PBS_CHECKPOINT           ( __CRYSTAX_SC_BASE | 0x0003 )
#define _SC_2_PBS_LOCATE               ( __CRYSTAX_SC_BASE | 0x0004 )
#define _SC_2_PBS_MESSAGE              ( __CRYSTAX_SC_BASE | 0x0005 )
#define _SC_2_PBS_TRACK                ( __CRYSTAX_SC_BASE | 0x0006 )

#define _SC_ADVISORY_INFO              ( __CRYSTAX_SC_BASE | 0x0007 )
#define _SC_BARRIERS                   ( __CRYSTAX_SC_BASE | 0x0008 )
#define _SC_CLOCK_SELECTION            ( __CRYSTAX_SC_BASE | 0x0009 )
#define _SC_CPUTIME                    ( __CRYSTAX_SC_BASE | 0x000a )
#define _SC_HOST_NAME_MAX              ( __CRYSTAX_SC_BASE | 0x000b )
#define _SC_IPV6                       ( __CRYSTAX_SC_BASE | 0x000c )
#define _SC_RAW_SOCKETS                ( __CRYSTAX_SC_BASE | 0x000d )
#define _SC_READER_WRITER_LOCKS        ( __CRYSTAX_SC_BASE | 0x000e )
#define _SC_REGEXP                     ( __CRYSTAX_SC_BASE | 0x000f )
#define _SC_SHELL                      ( __CRYSTAX_SC_BASE | 0x0010 )
#define _SC_SPAWN                      ( __CRYSTAX_SC_BASE | 0x0011 )
#define _SC_SPIN_LOCKS                 ( __CRYSTAX_SC_BASE | 0x0012 )
#define _SC_SPORADIC_SERVER            ( __CRYSTAX_SC_BASE | 0x0013 )
#define _SC_SS_REPL_MAX                ( __CRYSTAX_SC_BASE | 0x0014 )
#define _SC_SYMLOOP_MAX                ( __CRYSTAX_SC_BASE | 0x0015 )
#define _SC_THREAD_CPUTIME             ( __CRYSTAX_SC_BASE | 0x0016 )
#define _SC_THREAD_PROCESS_SHARED      ( __CRYSTAX_SC_BASE | 0x0017 )
#define _SC_THREAD_ROBUST_PRIO_INHERIT ( __CRYSTAX_SC_BASE | 0x0018 )
#define _SC_THREAD_ROBUST_PRIO_PROTECT ( __CRYSTAX_SC_BASE | 0x0019 )
#define _SC_THREAD_SPORADIC_SERVER     ( __CRYSTAX_SC_BASE | 0x001a )
#define _SC_TIMEOUTS                   ( __CRYSTAX_SC_BASE | 0x001b )
#define _SC_TRACE                      ( __CRYSTAX_SC_BASE | 0x001c )
#define _SC_TRACE_EVENT_FILTER         ( __CRYSTAX_SC_BASE | 0x001d )
#define _SC_TRACE_EVENT_NAME_MAX       ( __CRYSTAX_SC_BASE | 0x001e )
#define _SC_TRACE_INHERIT              ( __CRYSTAX_SC_BASE | 0x001f )
#define _SC_TRACE_LOG                  ( __CRYSTAX_SC_BASE | 0x0020 )
#define _SC_TRACE_NAME_MAX             ( __CRYSTAX_SC_BASE | 0x0021 )
#define _SC_TRACE_SYS_MAX              ( __CRYSTAX_SC_BASE | 0x0022 )
#define _SC_TRACE_USER_EVENT_MAX       ( __CRYSTAX_SC_BASE | 0x0023 )
#define _SC_TYPED_MEMORY_OBJECTS       ( __CRYSTAX_SC_BASE | 0x0024 )
#define _SC_XOPEN_STREAMS              ( __CRYSTAX_SC_BASE | 0x0025 )
#define _SC_XOPEN_UUCP                 ( __CRYSTAX_SC_BASE | 0x0026 )

#endif /* __CRYSTAX_INCLUDE_SYS_SYSCONF_H_7D99640560394B7CB625C2C31C7DF312 */
