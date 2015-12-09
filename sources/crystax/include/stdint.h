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

#ifndef __CRYSTAX_INCLUDE_STDINT_H_3A899278F04C4423895BBD7A8E83C321
#define __CRYSTAX_INCLUDE_STDINT_H_3A899278F04C4423895BBD7A8E83C321

#define __LIBCRYSTAX_STDINT_H_INCLUDED 1

#include <crystax/id.h>
#include <sys/_null.h> /* for NULL */
#include <sys/limits.h>
#include <sys/stdint.h>

#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

#define INT_LEAST8_C(c)   INT8_C(c)
#define INT_FAST8_C(c)    INT8_C(c)

#define UINT_LEAST8_C(c)  UINT8_C(c)
#define UINT_FAST8_C(c)   UINT8_C(c)

#define INT_LEAST16_C(c)  INT16_C(c)
#define INT_FAST16_C(c)   INT32_C(c)

#define UINT_LEAST16_C(c) UINT16_C(c)
#define UINT_FAST16_C(c)  UINT32_C(c)

#define INT_LEAST32_C(c)  INT32_C(c)
#define INT_FAST32_C(c)   INT32_C(c)

#define UINT_LEAST32_C(c) UINT32_C(c)
#define UINT_FAST32_C(c)  UINT32_C(c)

#define INT_LEAST64_C(c)  INT64_C(c)
#define INT_FAST64_C(c)   INT64_C(c)

#define UINT_LEAST64_C(c) UINT64_C(c)
#define UINT_FAST64_C(c)  UINT64_C(c)

#define INTPTR_C(c)       INT32_C(c)
#define UINTPTR_C(c)      UINT32_C(c)
#define PTRDIFF_C(c)      INT32_C(c)

#define INTMAX_C(c)       INT64_C(c)
#define UINTMAX_C(c)      UINT64_C(c)

#endif /* !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS) */

#endif /* __CRYSTAX_INCLUDE_STDINT_H_3A899278F04C4423895BBD7A8E83C321 */
