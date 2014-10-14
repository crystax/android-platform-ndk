#include <complex.h>
#include <float.h>
#include <math.h>

#define BF(name)  long double complex name ## l (long double complex x) { return name((double complex)x); }

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
BF(csqrt);
BF(ctan);
BF(ctanh);

double cabs(double complex x)
{
    return hypot(creal(x), cimag(x));
}

float cabsf(float complex x)
{
    return hypotf(crealf(x), cimagf(x));
}

long double cabsl(long double complex x)
{
    return cabs((double complex)x);
}

double carg(double complex x)
{
    return atan(cimag(x) / creal(x));
}

float cargf(float complex x)
{
    return atanf(cimagf(x) / crealf(x));
}

long double cargl(long double complex x)
{
    return carg((double complex)x);
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
    return clog((double complex)x);
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
    return cpow((double complex)x, (double complex)y);
}
