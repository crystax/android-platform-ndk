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

/*
 * This file implements the functionality of <fenv.h> on platforms that
 * lack an FPU and use softfloat in libc for floating point.  To use it,
 * you must write an <fenv.h> that provides the following:
 *
 *   - a typedef for fenv_t, which may be an integer or struct type
 *   - a typedef for fexcept_t (XXX This file assumes fexcept_t is a
 *     simple integer type containing the exception mask.)
 *   - definitions of FE_* constants for the five exceptions and four
 *     rounding modes in IEEE 754, as described in fenv(3)
 *   - a definition, and the corresponding external symbol, for FE_DFL_ENV
 *   - a macro __set_env(env, flags, mask, rnd), which sets the given fenv_t
 *     from the exception flags, mask, and rounding mode
 *   - macros __env_flags(env), __env_mask(env), and __env_round(env), which
 *     extract fields from an fenv_t
 *   - a definition of __fenv_static
 *
 * If the architecture supports an optional FPU, it's recommended that you
 * define fenv_t and fexcept_t to match the hardware ABI.  Otherwise, it
 * doesn't matter how you define them.
 */

#include <fenv.h>
#include <signal.h>
#include <unistd.h>

#if __SOFTFP__

#define _FPSCR_ENABLE_SHIFT 8
#define _FPSCR_ENABLE_MASK (FE_ALL_EXCEPT << _FPSCR_ENABLE_SHIFT)

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

const fenv_t __fe_dfl_env = 0;

static int __softfloat_float_rounding_mode = 0;
static int __softfloat_float_exception_mask = 0;
static int __softfloat_float_exception_flags = 0;

void __softfloat_float_raise(int e)
{
    if ((e & __softfloat_float_exception_mask) == 0)
        return;
    kill(getpid(), SIGFPE);
}

int feclearexcept(int __excepts)
{
    __softfloat_float_exception_flags &= ~__excepts;
    return (0);
}

int fegetexceptflag(fexcept_t *__flagp, int __excepts)
{
    *__flagp = __softfloat_float_exception_flags & __excepts;
    return (0);
}

int fesetexceptflag(const fexcept_t *__flagp, int __excepts)
{
    __softfloat_float_exception_flags &= ~__excepts;
    __softfloat_float_exception_flags |= *__flagp & __excepts;
    return (0);
}

int feraiseexcept(int __excepts)
{
    __softfloat_float_raise(__excepts);
    return (0);
}

int fetestexcept(int __excepts)
{
    return (__softfloat_float_exception_flags & __excepts);
}

int fegetround(void)
{
    return (__softfloat_float_rounding_mode);
}

int fesetround(int __round)
{
    __softfloat_float_rounding_mode = __round;
    return (0);
}

int fegetenv(fenv_t *__envp)
{
    __set_env(*__envp, __softfloat_float_exception_flags,
            __softfloat_float_exception_mask, __softfloat_float_rounding_mode);
    return (0);
}

int feholdexcept(fenv_t *__envp)
{
    fegetenv(__envp);
    __softfloat_float_exception_flags = 0;
    __softfloat_float_exception_mask = 0;
    return (0);
}

int fesetenv(const fenv_t *__envp)
{
    __softfloat_float_exception_flags = __env_flags(*__envp);
    __softfloat_float_exception_mask = __env_mask(*__envp);
    __softfloat_float_rounding_mode = __env_round(*__envp);
    return (0);
}

int feupdateenv(const fenv_t *__envp)
{
    int __oflags = __softfloat_float_exception_flags;

    fesetenv(__envp);
    feraiseexcept(__oflags);
    return (0);
}

int feenableexcept(int __mask)
{
    int __omask = __softfloat_float_exception_mask;

    __softfloat_float_exception_mask |= __mask;
    return (__omask);
}

int fedisableexcept(int __mask)
{
    int __omask = __softfloat_float_exception_mask;

    __softfloat_float_exception_mask &= ~__mask;
    return (__omask);
}

int fegetexcept(void)
{
    return (__softfloat_float_exception_mask);
}

#endif /* __SOFTFP__ */
