#include <assert.h>
#include <stdio.h>
#include <complex.h>


volatile double      double_result;
volatile float       float_result;
volatile long double long_double_result;

volatile double      complex double_complex_result;
volatile float       complex float_complex_result;
volatile long double complex long_double_complex_result;

volatile double      complex double_complex_var      = (double)      1.1 + (double)      1.1*I;
volatile float       complex float_complex_var       = (float)       2.2 + (float)       2.2*I;
volatile long double complex long_double_complex_var = (long double) 3.3 + (long double) 3.3*I;


static void print_double_complex_result(const char*funcname);
static void print_float_complex_result(const char*funcname);
static void print_long_double_complex_result(const char*funcname);

static void print_double_result(const char*funcname);
static void print_float_result(const char*funcname);
static void print_long_double_result(const char*funcname);


int main()
{
    printf("sizeof(long double) = %zu\n", sizeof(long double));
#ifdef __ANDROID__
    assert(sizeof(long double) == sizeof(double));
#else
    assert(sizeof(long double) >= sizeof(double));
#endif

    /* 7.3.5 Trigonometric functions */
    /* 7.3.5.1 The cacos functions */
    double_complex_result = cacos(double_complex_var);
    print_double_complex_result("cacos");
    float_complex_result = cacosf(float_complex_var);
    print_float_complex_result("cacosf");
    long_double_complex_result = cacosl(long_double_complex_var);
    print_long_double_complex_result("cacosl");

    /* 7.3.5.2 The casin functions */
    double_complex_result = casin(double_complex_var);
    print_double_complex_result("casin");
    float_complex_result = casinf(float_complex_var);
    print_float_complex_result("casinf");
    long_double_complex_result = casinl(long_double_complex_var);
    print_long_double_complex_result("casinl");

    /* 7.3.5.3 The catan functions */
    double_complex_result = catan(double_complex_var);
    print_double_complex_result("catan");
    float_complex_result = catanf(float_complex_var);
    print_float_complex_result("catanf");
    long_double_complex_result = catanl(long_double_complex_var);
    print_long_double_complex_result("catanl");

    /* 7.3.5.4 The ccos functions */
    double_complex_result = ccos(double_complex_var);
    print_double_complex_result("ccos");
    float_complex_result = ccosf(float_complex_var);
    print_float_complex_result("ccosf");
    long_double_complex_result = ccosl(long_double_complex_var);
    print_long_double_complex_result("ccosl");

    /* 7.3.5.5 The csin functions */
    double_complex_result = csin(double_complex_var);
    print_double_complex_result("csin");
    float_complex_result = csinf(float_complex_var);
    print_float_complex_result("csinf");
    long_double_complex_result = csinl(long_double_complex_var);
    print_long_double_complex_result("csinl");

    /* 7.3.5.6 The ctan functions */
    double_complex_result = ctan(double_complex_var);
    print_double_complex_result("ctan");
    float_complex_result = ctanf(float_complex_var);
    print_float_complex_result("ctanf");
    long_double_complex_result = ctanl(long_double_complex_var);
    print_long_double_complex_result("ctanl");

    /* 7.3.6 Hyperbolic functions */
    /* 7.3.6.1 The cacosh functions */
    double_complex_result = cacosh(double_complex_var);
    print_double_complex_result("cacosh");
    float_complex_result = cacoshf(float_complex_var);
    print_float_complex_result("cacoshf");
    long_double_complex_result = cacoshl(long_double_complex_var);
    print_long_double_complex_result("cacoshl");

    /* 7.3.6.2 The casinh functions */
    double_complex_result = casinh(double_complex_var);
    print_double_complex_result("casinh");
    float_complex_result = casinhf(float_complex_var);
    print_float_complex_result("casinhf");
    long_double_complex_result = casinhl(long_double_complex_var);
    print_long_double_complex_result("casinhf");

    /* 7.3.6.3 The catanh functions */
    double_complex_result = catanh(double_complex_result);
    print_double_complex_result("catanh");
    float_complex_result = catanhf(float_complex_var);
    print_float_complex_result("catanhf");
    long_double_complex_result = catanhl(long_double_complex_var);
    print_long_double_complex_result("catanhl");

    /* 7.3.6.4 The ccosh functions */
    double_complex_result = ccosh(double_complex_var);
    print_double_complex_result("ccosh");
    float_complex_result = ccoshf(float_complex_var);
    print_float_complex_result("ccoshf");
    long_double_complex_result = ccoshl(long_double_complex_var);
    print_long_double_complex_result("ccoshl");

    /* 7.3.6.5 The csinh functions */
    double_complex_result = csinh(double_complex_var);
    print_double_complex_result("csinh");
    float_complex_result = csinhf(float_complex_var);
    print_float_complex_result("csinhf");
    long_double_complex_result = csinhl(long_double_complex_var);
    print_long_double_complex_result("csinhl");

    /* 7.3.6.6 The ctanh functions */
    double_complex_result = ctanh(double_complex_var);
    print_double_complex_result("ctanh");
    float_complex_result = ctanhf(float_complex_var);
    print_float_complex_result("ctanhf");
    long_double_complex_result = ctanhl(long_double_complex_var);
    print_long_double_complex_result("ctanhl");

    /* 7.3.7 Exponential and logarithmic functions */
    /* 7.3.7.1 The cexp functions */
    double_complex_result = cexp(double_complex_var);
    print_double_complex_result("cexp");
    float_complex_result = cexpf(float_complex_var);
    print_float_complex_result("cexpf");
    long_double_complex_result = cexpl(long_double_complex_var);
    print_long_double_complex_result("cexpl");

    /* 7.3.7.2 The clog functions */
    double_complex_result = clog(double_complex_var);
    print_double_complex_result("clog");
    float_complex_result = clogf(float_complex_var);
    print_float_complex_result("clogf");
    long_double_complex_result = clogl(long_double_complex_var);
    print_long_double_complex_result("clogl");

    /* 7.3.8 Power and absolute-value functions */
    /* 7.3.8.1 The cabs functions */
    double_result = cabs(double_complex_var);
    print_double_result("cabs");
    float_result = cabsf(float_complex_var);
    print_float_result("cabsf");
    long_double_result = cabsl(long_double_complex_var);
    print_long_double_result("cabsl");

    /* 7.3.8.2 The cpow functions */
    double_complex_result = cpow(double_complex_var, double_complex_var);
    print_double_complex_result("cpow");
    float_complex_result = cpowf(float_complex_var, float_complex_var);
    print_float_complex_result("cpowf");
    long_double_complex_result = cpowl(long_double_complex_var, long_double_complex_var);
    print_long_double_complex_result("cpowl");

    /* 7.3.8.3 The csqrt functions */
    double_complex_result = csqrt(double_complex_var);
    print_double_complex_result("csqrt");
    float_complex_result = csqrtf(float_complex_var);
    print_float_complex_result("csqrtf");
    long_double_complex_result = csqrtl(long_double_complex_var);
    print_long_double_complex_result("csqrtl");

    /* 7.3.9 Manipulation functions */
    /* 7.3.9.1 The carg functions */
    double_result = carg(double_complex_var);
    print_double_result("carg");
    float_result = cargf(float_complex_var);
    print_float_result("cargf");
    long_double_result = cargl(long_double_complex_var);
    print_long_double_result("cargl");

    /* 7.3.9.2 The cimag functions */
    double_result = cimag(double_complex_var);
    print_double_result("cimag");
    float_result = cimagf(float_complex_var);
    print_float_result("cimagf");
    long_double_result = cimagl(long_double_complex_var);
    print_long_double_result("cimagl");

    /* 7.3.9.3 The conj functions */
    double_complex_result = conj(double_complex_var);
    print_double_complex_result("conj");
    float_complex_result = conjf(float_complex_var);
    print_float_complex_result("conjf");
    long_double_complex_result = conjl(long_double_complex_var);
    print_long_double_complex_result("conjl");

    /* 7.3.9.4 The cproj functions */
    double_complex_result = cproj(double_complex_var);
    print_double_complex_result("cproj");
    float_complex_result = cprojf(float_complex_var);
    print_float_complex_result("cprojf");
    long_double_complex_result = cprojl(long_double_complex_var);
    print_long_double_complex_result("cprojl");

    /* 7.3.9.5 The creal functions */
    double_result = creal(double_complex_var);
    print_double_result("creal");
    float_result = crealf(float_complex_var);
    print_float_result("crealf");
    long_double_result = creall(long_double_complex_var);
    print_long_double_result("creall");

    return 0;
}

void print_double_complex_result(const char*funcname)
{
    printf("%s: %e + i%e\n", funcname, creal(double_complex_result), cimag(double_complex_result));
}

void print_float_complex_result(const char*funcname)
{
    printf("%s: %e + i%e\n", funcname, crealf(float_complex_result), cimagf(float_complex_result));
}

void print_long_double_complex_result(const char*funcname)
{
    printf("%s: %Le + i%Le\n", funcname, creall(long_double_complex_result), cimagl(long_double_complex_result));
}

void print_double_result(const char*funcname)
{
    printf("%s: %e\n", funcname, double_result);
}

void print_float_result(const char*funcname)
{
    printf("%s: %e\n", funcname, float_result);
}

void print_long_double_result(const char*funcname)
{
    printf("%s: %Le\n", funcname, long_double_result);
}
