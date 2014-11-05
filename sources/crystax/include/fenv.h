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

#ifndef __CRYSTAX_INCLUDE_FENV_H_C7DBD79B038240CB903BDCE8D17AE833
#define __CRYSTAX_INCLUDE_FENV_H_C7DBD79B038240CB903BDCE8D17AE833

#include <crystax/id.h>

#if !__SOFTFP__
#include <crystax/google/fenv.h>
#else /* __SOFTFP__ */

#define _FENV_H_

#include <sys/types.h>

__BEGIN_DECLS

typedef __uint32_t fenv_t;
typedef __uint32_t fexcept_t;

/* Default floating-point environment. */
extern const fenv_t __crystax_softfloat_fe_dfl_env;
#define FE_DFL_ENV (&__crystax_softfloat_fe_dfl_env);

/* Exception flags. */
#define FE_INVALID    0x01
#define FE_DIVBYZERO  0x02
#define FE_OVERFLOW   0x04
#define FE_UNDERFLOW  0x08
#define FE_INEXACT    0x10
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)
#define _FPSCR_ENABLE_SHIFT 8
#define _FPSCR_ENABLE_MASK (FE_ALL_EXCEPT << _FPSCR_ENABLE_SHIFT)

/* Rounding modes. */
#define FE_TONEAREST  0x0
#define FE_UPWARD     0x1
#define FE_DOWNWARD   0x2
#define FE_TOWARDZERO 0x3
#define _FPSCR_RMODE_SHIFT 22

#define _ROUND_MASK (FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO)

#define _FPUSW_SHIFT 16

#define __set_env(env, flags, mask, rnd) \
        env = ((flags) \
        | (mask)<<_FPUSW_SHIFT \
        | (rnd) << 24)

#define __env_flags(env) ((env) & FE_ALL_EXCEPT)
#define __env_mask(env)  (((env) >> _FPUSW_SHIFT) & FE_ALL_EXCEPT)
#define __env_round(env) (((env) >> 24) & _ROUND_MASK)

#define __fenv_static static
#include <crystax/freebsd/lib/msun/src/fenv-softfloat.h>

__END_DECLS

#endif /* __SOFTFP__ */

#endif /* __CRYSTAX_INCLUDE_FENV_H_C7DBD79B038240CB903BDCE8D17AE833 */
