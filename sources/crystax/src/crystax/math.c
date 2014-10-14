#include <math.h>

#define BF(name)  long double name (long double x) { return __builtin_ ## name (x); }
#define BF2(name) long double name (long double x, long double y) { return __builtin_ ## name ( x, y ); }

BF(coshl);
BF(expl);
BF(log10l);
BF(logl);
BF(sinhl);
BF(sqrtl);
BF(tanhl);
BF2(atan2l);
BF2(fmodl);
BF2(hypotl);
BF2(powl);

long double modfl(long double x, long double *y) { return __builtin_modfl(x, y); }

void sincos(double x, double *s, double *c)
{
    return __builtin_sincos(x, s, c);
}

void sincosf(float x, float *s, float *c)
{
    return __builtin_sincosf(x, s, c);
}

void sincosl(long double x, long double *s, long double *c)
{
    return __builtin_sincosl(x, s, c);
}
