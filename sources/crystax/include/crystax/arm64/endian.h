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

#ifndef __CRYSTAX_INCLUDE_ARM64_ENDIAN_H_E421366A61D2402F8C5C0B02844266E8
#define __CRYSTAX_INCLUDE_ARM64_ENDIAN_H_E421366A61D2402F8C5C0B02844266E8

#include <crystax/id.h>
#include <sys/_types.h>

#define _LITTLE_ENDIAN  1234
#define _BIG_ENDIAN 4321
#define _PDP_ENDIAN 3412

#if defined(__AARCH64EB__)
#define _BYTE_ORDER _BIG_ENDIAN
#else
#define _BYTE_ORDER _LITTLE_ENDIAN
#endif

#if __BSD_VISIBLE
#define LITTLE_ENDIAN   _LITTLE_ENDIAN
#define BIG_ENDIAN      _BIG_ENDIAN
#define PDP_ENDIAN      _PDP_ENDIAN
#define BYTE_ORDER      _BYTE_ORDER
#endif

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define _QUAD_HIGHWORD 1
#define _QUAD_LOWWORD  0
#define __ntohl(x) __bswap32(x)
#define __ntohs(x) __bswap16(x)
#define __htonl(x) __bswap32(x)
#define __htons(x) __bswap32(x)
#else
#define _QUAD_HIGHWORD 0
#define _QUAD_LOWWORD  1
#define __ntohl(x) ((uint32_t)(x))
#define __ntohs(x) ((uint16_t)(x))
#define __htonl(x) ((uint32_t)(x))
#define __htons(x) ((uint16_t)(x))
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

static __inline __uint16_t
__bswap16_var(__uint16_t x)
{
    register __uint16_t _x = x;
    __asm volatile ("rev16 %0, %0" : "+r" (_x));
    return _x;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#define __bswap32_var(x) __builtin_bswap32(x)
#define __bswap64_var(x) __builtin_bswap64(x)

#ifdef __OPTIMIZE__

#define __bswap16_constant(x) __statement({             \
        __uint16_t __swap16gen_x = (x);                 \
        (__uint16_t)((__swap16gen_x & 0xff) << 8 |      \
                     (__swap16gen_x & 0xff00) >> 8);    \
    })

#define __bswap32_constant(x) __statement({               \
        __uint32_t __swap32gen_x = (x);                   \
        (__uint32_t)((__swap32gen_x & 0xff) << 24 |       \
                     (__swap32gen_x & 0xff00) << 8 |      \
                     (__swap32gen_x & 0xff0000) >> 8 |    \
                     (__swap32gen_x & 0xff000000) >> 24); \
    })

#define __bswap64_constant(x) __statement({                          \
        __uint64_t __swap64gen_x = (x);                              \
        (__uint64_t)((__swap64gen_x & 0xff) << 56 |                  \
                     (__swap64gen_x & 0xff00ULL) << 40 |             \
                     (__swap64gen_x & 0xff0000ULL) << 24 |           \
                     (__swap64gen_x & 0xff000000ULL) << 8 |          \
                     (__swap64gen_x & 0xff00000000ULL) >> 8 |        \
                     (__swap64gen_x & 0xff0000000000ULL) >> 24 |     \
                     (__swap64gen_x & 0xff000000000000ULL) >> 40 |   \
                     (__swap64gen_x & 0xff00000000000000ULL) >> 56); \
    })

#define __bswap16(x) \
    (__builtin_constant_p(x) ? __bswap16_constant(x) : __bswap16_var(x))

#define __bswap32(x) \
    (__builtin_constant_p(x) ? __bswap32_constant(x) : __bswap32_var(x))

#define __bswap64(x) \
    (__builtin_constant_p(x) ? __bswap64_constant(x) : __bswap64_var(x))

#else

#define __bswap16(x) __bswap16_var(x)
#define __bswap32(x) __bswap32_var(x)
#define __bswap64(x) __bswap64_var(x)

#endif /* __OPTIMIZE__ */

#endif /* __CRYSTAX_INCLUDE_ARM64_ENDIAN_H_E421366A61D2402F8C5C0B02844266E8 */
