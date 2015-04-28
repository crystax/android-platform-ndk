/*
 * Copyright (c) 2011-2015 CrystaX .NET.
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

#ifndef __CRYSTAX_INCLUDE_CRYSTAX_ARM_FENV_H_5DD72E92B8A449719F0B902A909BCB18
#define __CRYSTAX_INCLUDE_CRYSTAX_ARM_FENV_H_5DD72E92B8A449719F0B902A909BCB18

#include <crystax/id.h>

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

typedef __uint32_t fenv_t;
typedef __uint32_t fexcept_t;

/* Exception flags. */
#define FE_INVALID    0x01
#define FE_DIVBYZERO  0x02
#define FE_OVERFLOW   0x04
#define FE_UNDERFLOW  0x08
#define FE_INEXACT    0x10
#define FE_ALL_EXCEPT (FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes. */
#define FE_TONEAREST  0x0
#define FE_UPWARD     0x1
#define FE_DOWNWARD   0x2
#define FE_TOWARDZERO 0x3

/* Default floating-point environment. */
extern const fenv_t __fe_dfl_env;
#define FE_DFL_ENV (&__fe_dfl_env)

int fegetenv(fenv_t* __envp);
int fesetenv(const fenv_t* __envp);

int feclearexcept(int __excepts);
int fegetexceptflag(fexcept_t* __flagp, int __excepts);
int fesetexceptflag(const fexcept_t* __flagp, int __excepts);
int feraiseexcept(int __excepts);
int fetestexcept(int __excepts);
int fegetround(void);
int fesetround(int __round);
int feholdexcept(fenv_t* __envp);
int feupdateenv(const fenv_t* __envp);

#if __BSD_VISIBLE

int feenableexcept(int __mask);
int fedisableexcept(int __mask);
int fegetexcept(void);

#endif /* __BSD_VISIBLE */

__END_DECLS

#endif /* __CRYSTAX_INCLUDE_CRYSTAX_ARM_FENV_H_5DD72E92B8A449719F0B902A909BCB18 */
