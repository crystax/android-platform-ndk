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

#ifndef __CRYSTAX_INCLUDE_MIPS64_ENDIAN_H_3CC1D29D10C64ADF995C0699D4C6471C
#define __CRYSTAX_INCLUDE_MIPS64_ENDIAN_H_3CC1D29D10C64ADF995C0699D4C6471C

#include <crystax/id.h>
#include <sys/_types.h>

#define _LITTLE_ENDIAN  1234
#define _BIG_ENDIAN 4321
#define _PDP_ENDIAN 3412

#if defined(__MIPSEB__)
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

#define __bswap16_gen(x)      __statement({               \
        __uint16_t __swap16gen_x = (x);                   \
        (__uint16_t)((__swap16gen_x & 0xff) << 8 |        \
                     (__swap16gen_x & 0xff00) >> 8);      \
    })

#define __bswap32_gen(x) __statement({                    \
        __uint32_t __swap32gen_x = (x);                   \
        (__uint32_t)((__swap32gen_x & 0xff) << 24 |       \
                     (__swap32gen_x & 0xff00) << 8 |      \
                     (__swap32gen_x & 0xff0000) >> 8 |    \
                     (__swap32gen_x & 0xff000000) >> 24); \
    })

#define __bswap64_gen(x) __statement({                               \
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

#define __bswap16_constant(x) __bswap16_gen(x)
#define __bswap32_constant(x) __bswap32_gen(x)
#define __bswap64_constant(x) __bswap64_gen(x)

#if defined(__mips_isa_rev) && (__mips_isa_rev >= 2)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#endif

static __inline __uint16_t
__bswap16_var(__uint16_t x)
{
    register __uint16_t _x = x;
    register __uint16_t _r;
    __asm volatile ("wsbh %0, %1" : "=r" (_r) : "r" (_x));
    return _r;
}

static __inline __uint32_t
__bswap32_var(__uint32_t x)
{
    register __uint32_t _x = x;
    register __uint32_t _r;
    __asm volatile ("wsbh %0, %1; rotr %0, %0, 16" : "=r" (_r) : "r" (_x));
    return _r;
}

static __inline __uint64_t
__bswap64_var(__uint64_t x)
{
    __uint64_t _x = x;
    return ((__uint64_t) __bswap32_var(_x >> 32)) |
        ((__uint64_t) __bswap32_var(_x & 0xffffffff) << 32);
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#else /* !defined(__mips_isa_rev) || (__mips_isa_rev < 2) */

#define __bswap16_var(x) __bswap16_gen(x)
#define __bswap32_var(x) __bswap32_gen(x)
#define __bswap64_var(x) __bswap64_gen(x)

#endif /* !defined(__mips_isa_rev) || (__mips_isa_rev < 2) */

#ifdef __OPTIMIZE__

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

#endif /* __CRYSTAX_INCLUDE_MIPS64_ENDIAN_H_3CC1D29D10C64ADF995C0699D4C6471C */
