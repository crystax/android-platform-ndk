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

#include <complex.h>
#include <float.h>
#include <math.h>

#if !defined(__LDBL_MANT_DIG__)
#error __LDBL_MANT_DIG__ not defined
#endif

#if __LDBL_MANT_DIG__ <= 53
#define WARN_IMPRECISE(x)
#else
#define WARN_IMPRECISE(x) __warn_references(x, # x " has lower than advertised precision");
#endif

#define BF(name) \
    long double complex name ## l (long double complex x) { return name((double complex)x); }; \
    WARN_IMPRECISE(name)

BF(cacos);
BF(cacosh);
BF(casin);
BF(casinh);
BF(catan);
BF(catanh);
BF(ccos);
BF(ccosh);
BF(cexp);
BF(csin);
BF(csinh);
BF(ctan);
BF(ctanh);

long double cargl(long double complex x)
{
    return atan2l(cimagl(x), creall(x));
}

double complex clog(double complex x)
{
    return log(cabs(x)) + I * carg(x);
}

float complex clogf(float complex x)
{
    return logf(cabsf(x)) + I * cargf(x);
}

long double complex clogl(long double complex x)
{
    return logl(cabsl(x)) + I * cargl(x);
}

double complex cpow(double complex x, double complex y)
{
    return cexp(y * clog(x));
}

float complex cpowf(float complex x, float complex y)
{
    return cexpf(y * clogf(x));
}

long double complex cpowl(long double complex x, long double complex y)
{
    return cexpl(y * clogl(x));
}
