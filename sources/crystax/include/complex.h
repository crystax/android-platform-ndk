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

/* $NetBSD: complex.h,v 1.4 2013/01/28 23:19:50 matt Exp $ */

/*
 * Written by Matthias Drochner.
 * Public domain.
 */

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


#ifndef	CRYSTAX_COMPLEX_H
#define	CRYSTAX_COMPLEX_H

#define complex _Complex
#define _Complex_I 1.0fi
#define I _Complex_I

#include <sys/cdefs.h>

__BEGIN_DECLS

/* Because Android does not supports long double type (it just defines
 * it as a typedef to double) long double variants of a complex
 * functions here are implemented using simple type cast. */

/* 7.3.5 Trigonometric functions */
/* 7.3.5.1 The cacos functions */
double complex cacos(double complex);
float complex cacosf(float complex);
static inline long double complex cacosl(long double complex c)
{
    return (long double complex) cacos((double complex) c);
}

/* 7.3.5.2 The casin functions */
double complex casin(double complex);
float complex casinf(float complex);
static inline long double complex casinl(long double complex c)
{
    return (long double complex) casin((double complex) c);
}

/* 7.3.5.3 The catan functions */
double complex catan(double complex);
float complex catanf(float complex);
static inline long double complex catanl(long double complex c)
{
    return (long double complex) catan((double complex) c);
}

/* 7.3.5.4 The ccos functions */
double complex ccos(double complex);
float complex ccosf(float complex);
static inline long double complex ccosl(long double complex c)
{
    return (long double complex) ccos((double complex) c);
}

/* 7.3.5.5 The csin functions */
double complex csin(double complex);
float complex csinf(float complex);
static inline long double complex csinl(long double complex c)
{
    return (long double complex) csin((double complex) c);
}

/* 7.3.5.6 The ctan functions */
double complex ctan(double complex);
float complex ctanf(float complex);
static inline long double complex ctanl(long double complex c)
{
    return (long double complex) ctan((double complex) c);
}

/* 7.3.6 Hyperbolic functions */
/* 7.3.6.1 The cacosh functions */
double complex cacosh(double complex);
float complex cacoshf(float complex);
static inline long double complex cacoshl(long double complex c)
{
    return (long double complex) cacosh((double complex) c);
}

/* 7.3.6.2 The casinh functions */
double complex casinh(double complex);
float complex casinhf(float complex);
static inline long double complex casinhl(long double complex c)
{
    return (long double complex) casinh((double complex) c);
}

/* 7.3.6.3 The catanh functions */
double complex catanh(double complex);
float complex catanhf(float complex);
static inline long double complex catanhl(long double complex c)
{
    return (long double complex) catanh((double complex) c);
}

/* 7.3.6.4 The ccosh functions */
double complex ccosh(double complex);
float complex ccoshf(float complex);
static inline long double complex ccoshl(long double complex c)
{
    return (long double complex) ccosh((double complex) c);
}

/* 7.3.6.5 The csinh functions */
double complex csinh(double complex);
float complex csinhf(float complex);
static inline long double complex csinhl(long double complex c)
{
    return (long double complex) csinh((double complex) c);
}

/* 7.3.6.6 The ctanh functions */
double complex ctanh(double complex);
float complex ctanhf(float complex);
static inline long double complex ctanhl(long double complex c)
{
    return (long double complex) ctanh((double complex) c);
}

/* 7.3.7 Exponential and logarithmic functions */
/* 7.3.7.1 The cexp functions */
double complex cexp(double complex);
float complex cexpf(float complex);
static inline long double complex cexpl(long double complex c)
{
    return (long double complex) cexp((double complex) c);
}

/* 7.3.7.2 The clog functions */
double complex clog(double complex);
float complex clogf(float complex);
static inline long double complex clogl(long double complex c)
{
    return (long double complex) clog((double complex) c);
}

/* 7.3.8 Power and absolute-value functions */
/* 7.3.8.1 The cabs functions */
/* #ifndef __LIBM0_SOURCE__ */
/* /\* avoid conflict with historical cabs(struct complex) *\/ */
/* double cabs(double complex) __RENAME(__c99_cabs); */
/* float cabsf(float complex) __RENAME(__c99_cabsf); */
/* long double cabsl(long double complex) __RENAME(__c99_cabsl); */
/* #endif */
double cabs(double complex);
float cabsf(float complex);
static inline long double cabsl(long double complex c)
{
    return (long double) cabs((double complex) c);
}

/* 7.3.8.2 The cpow functions */
double complex cpow(double complex, double complex);
float complex cpowf(float complex, float complex);
static inline long double complex cpowl(long double complex base, long double complex power)
{
    return (long double complex) cpow((double complex) base, (double complex) power);
}

/* 7.3.8.3 The csqrt functions */
double complex csqrt(double complex);
float complex csqrtf(float complex);
static inline long double complex csqrtl(long double complex c)
{
    return (long double complex) csqrt((double complex) c);
}

/* 7.3.9 Manipulation functions */
/* 7.3.9.1 The carg functions */
double carg(double complex);
float cargf(float complex);
static inline long double cargl(long double complex c)
{
    return (long double) carg((double complex) c);
}

/* 7.3.9.2 The cimag functions */
double cimag(double complex);
float cimagf(float complex);
long double cimagl(long double complex);

/* 7.3.9.3 The conj functions */
double complex conj(double complex);
float complex conjf(float complex);
long double complex conjl(long double complex);

/* 7.3.9.4 The cproj functions */
double complex cproj(double complex);
float complex cprojf(float complex);
long double complex cprojl(long double complex);

/* 7.3.9.5 The creal functions */
double creal(double complex);
float crealf(float complex);
long double creall(long double complex);

__END_DECLS

#endif	/* ! CRYSTAX_COMPLEX_H */
