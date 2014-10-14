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

#ifndef __CRYSTAX_INCLUDE_SYS_ENDIAN_H_B013EA419E364C5987ED776492BED803
#define __CRYSTAX_INCLUDE_SYS_ENDIAN_H_B013EA419E364C5987ED776492BED803

#include <crystax/id.h>

#ifndef _BYTE_ORDER
#error '_BYTE_ORDER' not defined
#endif

#ifndef _LITTLE_ENDIAN
#error '_LITTLE_ENDIAN' not defined
#endif

#ifndef _BIG_ENDIAN
#error '_BIG_ENDIAN' not defined
#endif

#ifndef __BYTE_ORDER
#define __BYTE_ORDER _BYTE_ORDER
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN _BIG_ENDIAN
#endif

#if __BSD_VISIBLE

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define betoh16(x) bswap16(x)
#define betoh32(x) bswap32(x)
#define betoh64(x) bswap64(x)

#define letoh16(x) ((uint16_t)(x))
#define letoh32(x) ((uint32_t)(x))
#define letoh64(x) ((uint64_t)(x))

#define ntohq(x) bswap64(x)
#define htonq(x) bswap64(x)
#endif /* _BYTE_ORDER */

#if _BYTE_ORDER == _BIG_ENDIAN
#define betoh16(x) ((uint16_t)(x))
#define betoh32(x) ((uint32_t)(x))
#define betoh64(x) ((uint64_t)(x))

#define letoh16(x) bswap16(x)
#define letoh32(x) bswap32(x)
#define letoh64(x) bswap64(x)

#define ntohq(x) ((uint64_t)(x))
#define htonq(x) ((uint64_t)(x))
#endif /* _BYTE_ORDER */

#define NTOHL(x) (x) = ntohl((uint32_t)(x))
#define NTOHS(x) (x) = ntohs((uint16_t)(x))
#define HTONL(x) (x) = htonl((uint32_t)(x))
#define HTONS(x) (x) = htons((uint16_t)(x))

#endif /* __BSD_VISIBLE */

#define __swap16(x) __bswap16(x)
#define __swap32(x) __bswap32(x)
#define __swap64(x) __bswap64(x)

#define __swap16md(x) __bswap16_var(x)
#define __swap32md(x) __bswap32_var(x)
#define __swap64md(x) __bswap64_var(x)

#endif /* __CRYSTAX_INCLUDE_SYS_ENDIAN_H_B013EA419E364C5987ED776492BED803 */
