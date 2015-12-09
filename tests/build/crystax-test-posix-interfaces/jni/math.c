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

#include <math.h>

#include "gen/math.inc"

#include "helper.h"

#define CHECK(type) type JOIN(math_var_, __LINE__)

CHECK(float_t);
CHECK(double_t);

/* Check is signgam is defined */
void check_signgam()
{
    int check_signgam_defined = signgam;
    (void)check_signgam_defined;
    int *check_signgam_is_variable = &signgam;
    (void)check_signgam_is_variable;
}

#if defined(FLT_EVAL_METHOD)

#if FLT_EVAL_METHOD == 0
CTASSERT(sizeof(float_t)  == sizeof(float));
CTASSERT(sizeof(double_t) == sizeof(double));
#elif FLT_EVAL_METHOD == 1
CTASSERT(sizeof(float_t)  == sizeof(double));
CTASSERT(sizeof(double_t) == sizeof(double));
#elif FLT_EVAL_METHOD == 2
CTASSERT(sizeof(float_t)  == sizeof(long double));
CTASSERT(sizeof(double_t) == sizeof(long double));
#endif

#endif /* defined(FLT_EVAL_METHOD) */

#define Z 0.0

void check_math_functions()
{
    (void)acos(Z);
    (void)acosf(Z);
    (void)acosh(Z);
    (void)acoshf(Z);
    (void)acoshl(Z);
    (void)acosl(Z);
    (void)asin(Z);
    (void)asinf(Z);
    (void)asinh(Z);
    (void)asinhf(Z);
    (void)asinhl(Z);
    (void)asinl(Z);
    (void)atan(Z);
    (void)atan2(Z, Z);
    (void)atan2f(Z, Z);
    (void)atan2l(Z, Z);
    (void)atanf(Z);
    (void)atanh(Z);
    (void)atanhf(Z);
    (void)atanhl(Z);
    (void)atanl(Z);
    (void)cbrt(Z);
    (void)cbrtf(Z);
    (void)cbrtl(Z);
    (void)ceil(Z);
    (void)ceilf(Z);
    (void)ceill(Z);
    (void)copysign(Z, Z);
    (void)copysignf(Z, Z);
    (void)copysignl(Z, Z);
    (void)cos(Z);
    (void)cosf(Z);
    (void)cosh(Z);
    (void)coshf(Z);
    (void)coshl(Z);
    (void)cosl(Z);
    (void)erf(Z);
    (void)erfc(Z);
    (void)erfcf(Z);
    (void)erfcl(Z);
    (void)erff(Z);
    (void)erfl(Z);
    (void)exp(Z);
    (void)exp2(Z);
    (void)exp2f(Z);
    (void)exp2l(Z);
    (void)expf(Z);
    (void)expl(Z);
    (void)expm1(Z);
    (void)expm1f(Z);
    (void)expm1l(Z);
    (void)fabs(Z);
    (void)fabsf(0.0f);
    (void)fabsl(Z);
    (void)fdim(Z, Z);
    (void)fdimf(Z, Z);
    (void)fdiml(Z, Z);
    (void)floor(Z);
    (void)floorf(Z);
    (void)floorl(Z);
    (void)fma(Z, Z, Z);
    (void)fmaf(Z, Z, Z);
    (void)fmal(Z, Z, Z);
    (void)fmax(Z, Z);
    (void)fmaxf(Z, Z);
    (void)fmaxl(Z, Z);
    (void)fmin(Z, Z);
    (void)fminf(Z, Z);
    (void)fminl(Z, Z);
    (void)fmod(Z, Z);
    (void)fmodf(Z, Z);
    (void)fmodl(Z, Z);
    (void)frexp(Z, (int*)0);
    (void)frexpf(Z, (int*)0);
    (void)frexpl(Z, (int*)0);
    (void)hypot(Z, Z);
    (void)hypotf(Z, Z);
    (void)hypotl(Z, Z);
    (void)ilogb(Z);
    (void)ilogbf(Z);
    (void)ilogbl(Z);
#if __XSI_VISIBLE
    (void)j0(Z);
    (void)j1(Z);
    (void)jn(0, Z);
#endif
    (void)ldexp(Z, 0);
    (void)ldexpf(Z, 0);
    (void)ldexpl(Z, 0);
    (void)lgamma(Z);
    (void)lgammaf(Z);
    (void)lgammal(Z);
    (void)llrint(Z);
    (void)llrintf(Z);
    (void)llrintl(Z);
    (void)llround(Z);
    (void)llroundf(Z);
    (void)llroundl(Z);
    (void)log(Z);
    (void)log10(Z);
    (void)log10f(Z);
    (void)log10l(Z);
    (void)log1p(Z);
    (void)log1pf(Z);
    (void)log1pl(Z);
    (void)log2(Z);
    (void)log2f(Z);
    (void)log2l(Z);
    (void)logb(Z);
    (void)logbf(Z);
    (void)logbl(Z);
    (void)logf(Z);
    (void)logl(Z);
    (void)lrint(Z);
    (void)lrintf(Z);
    (void)lrintl(Z);
    (void)lround(Z);
    (void)lroundf(Z);
    (void)lroundl(Z);
    (void)modf(Z, (double *)0);
    (void)modff(Z, (float *)0);
    (void)modfl(Z, (long double *)0);
    (void)nan("1");
    (void)nanf("1");
    (void)nanl("1");
    (void)nearbyint(Z);
    (void)nearbyintf(Z);
    (void)nearbyintl(Z);
    (void)nextafter(Z, Z);
    (void)nextafterf(Z, Z);
    (void)nextafterl(Z, Z);
    (void)nexttoward(Z, Z);
    (void)nexttowardf(Z, Z);
    (void)nexttowardl(Z, Z);
    (void)pow(Z, Z);
    (void)powf(Z, Z);
    (void)powl(Z, Z);
    (void)remainder(Z, Z);
    (void)remainderf(Z, Z);
    (void)remainderl(Z, Z);
    (void)remquo(Z, Z, (int*)0);
    (void)remquof(Z, Z, (int*)0);
    (void)remquol(Z, Z, (int*)0);
    (void)rint(Z);
    (void)rintf(Z);
    (void)rintl(Z);
    (void)round(Z);
    (void)roundf(Z);
    (void)roundl(Z);
#if __XSI_VISIBLE
    (void)scalb(Z, Z);
#endif
    (void)scalbln(Z, 0);
    (void)scalblnf(Z, 0);
    (void)scalblnl(Z, 0);
    (void)scalbn(Z, 0);
    (void)scalbnf(Z, 0);
    (void)scalbnl(Z, 0);
    (void)sin(Z);
    (void)sinf(Z);
    (void)sinh(Z);
    (void)sinhf(Z);
    (void)sinhl(Z);
    (void)sinl(Z);
    (void)sqrt(Z);
    (void)sqrtf(Z);
    (void)sqrtl(Z);
    (void)tan(Z);
    (void)tanf(Z);
    (void)tanh(Z);
    (void)tanhf(Z);
    (void)tanhl(Z);
    (void)tanl(Z);
    (void)tgamma(Z);
    (void)tgammaf(Z);
    (void)tgammal(Z);
    (void)trunc(Z);
    (void)truncf(Z);
    (void)truncl(Z);
#if __XSI_VISIBLE
    (void)y0(Z);
    (void)y1(Z);
    (void)yn(0, Z);
#endif
}
