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

#ifndef __CRYSTAX_INCLUDE_MIPS64_GD_QNAN_H_1DA89F97A9934AE5BC35666909AF03D1
#define __CRYSTAX_INCLUDE_MIPS64_GD_QNAN_H_1DA89F97A9934AE5BC35666909AF03D1

#include <crystax/id.h>

/*
 * WARNING!!!
 * These values are stolen from another sources and most likely will not work
 * on real MIPS64 device.
 * TODO: build contrib/gdtoa/qnan.c, run it on MIPS64 device and fill proper values here
*/

#define f_QNAN  0x7fbfffff
#define d_QNAN0 0x7ff7ffff
#define d_QNAN1 0xffffffff

#define ld_QNAN0 0x7fff8000
#define ld_QNAN1 0x00000000
#define ld_QNAN2 0x00000000
#define ld_QNAN3 0x00000000

#define ldus_QNAN0 0xffff
#define ldus_QNAN1 0xffff
#define ldus_QNAN2 0xffff
#define ldus_QNAN3 0xffff
#define ldus_QNAN4 0xffff


#endif /* __CRYSTAX_INCLUDE_MIPS64_GD_QNAN_H_1DA89F97A9934AE5BC35666909AF03D1 */
