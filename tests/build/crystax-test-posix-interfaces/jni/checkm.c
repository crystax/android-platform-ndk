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

#if __ANDROID__

#include <sys/types.h>
#include <stdint.h>
#include <machine/_align.h>

#include "helper.h"

#ifndef _ALIGNBYTES
#error  _ALIGNBYTES not defined
#endif

#ifndef _ALIGN
#error  _ALIGN not defined
#endif

#ifndef __WORDSIZE
#error  __WORDSIZE not defined
#endif

#if __LP64__
CTASSERT(__WORDSIZE == 64);
#else
CTASSERT(__WORDSIZE == 32);
#endif

#if __LP64__
CTASSERT(sizeof(register_t) == 8);
#else
CTASSERT(sizeof(register_t) == 4);
#endif

#if __LP64__
CTASSERT(_ALIGNBYTES == 7);
#else
CTASSERT(_ALIGNBYTES == 3);
#endif

#if __LP64__
CTASSERT(_ALIGN(0)   == 0);
CTASSERT(_ALIGN(1)   == 8);
CTASSERT(_ALIGN(2)   == 8);
CTASSERT(_ALIGN(3)   == 8);
CTASSERT(_ALIGN(4)   == 8);
CTASSERT(_ALIGN(5)   == 8);
CTASSERT(_ALIGN(9)   == 16);
CTASSERT(_ALIGN(12)  == 16);
CTASSERT(_ALIGN(13)  == 16);
#else
CTASSERT(_ALIGN(0)   == 0);
CTASSERT(_ALIGN(1)   == 4);
CTASSERT(_ALIGN(2)   == 4);
CTASSERT(_ALIGN(3)   == 4);
CTASSERT(_ALIGN(4)   == 4);
CTASSERT(_ALIGN(5)   == 8);
CTASSERT(_ALIGN(9)   == 12);
CTASSERT(_ALIGN(12)  == 12);
CTASSERT(_ALIGN(13)  == 16);
#endif

#endif /* __ANDROID__ */
