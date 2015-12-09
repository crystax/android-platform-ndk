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

#ifndef complex
#error 'complex' not defined
#endif

#ifndef _Complex_I
#error '_Complex_I' not defined
#endif

#ifndef I
#error 'I' not defined
#endif

#ifdef _Imaginary_I
#ifndef imaginary
#error 'imaginary' not defined
#endif
#endif

void check_complex_functions(float complex f, double complex d, long double complex ld)
{
    (void)cabs(d);
    (void)cabsf(f);
    (void)cabsl(ld);
    (void)cacos(d);
    (void)cacosf(f);
    (void)cacosh(d);
    (void)cacoshf(f);
    (void)cacoshl(ld);
    (void)cacosl(ld);
    (void)carg(d);
    (void)cargf(f);
    (void)cargl(ld);
    (void)casin(d);
    (void)casinf(f);
    (void)casinh(d);
    (void)casinhf(f);
    (void)casinhl(ld);
    (void)casinl(ld);
    (void)catan(d);
    (void)catanf(f);
    (void)catanh(d);
    (void)catanhf(f);
    (void)catanhl(ld);
    (void)catanl(ld);
    (void)ccos(d);
    (void)ccosf(f);
    (void)ccosh(d);
    (void)ccoshf(f);
    (void)ccoshl(ld);
    (void)ccosl(ld);
    (void)cexp(d);
    (void)cexpf(f);
    (void)cexpl(ld);
    (void)cimag(d);
    (void)cimagf(f);
    (void)cimagl(ld);
    (void)clog(d);
    (void)clogf(f);
    (void)clogl(ld);
    (void)conj(d);
    (void)conjf(f);
    (void)conjl(ld);
    (void)cpow(d, d);
    (void)cpowf(f, f);
    (void)cpowl(ld, ld);
    (void)cproj(d);
    (void)cprojf(f);
    (void)cprojl(ld);
    (void)creal(d);
    (void)crealf(f);
    (void)creall(ld);
    (void)csin(d);
    (void)csinf(f);
    (void)csinh(d);
    (void)csinhf(f);
    (void)csinhl(ld);
    (void)csinl(ld);
    (void)csqrt(d);
    (void)csqrtf(f);
    (void)csqrtl(ld);
    (void)ctan(d);
    (void)ctanf(f);
    (void)ctanh(d);
    (void)ctanhf(f);
    (void)ctanhl(ld);
    (void)ctanl(ld);
}
