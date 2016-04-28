#if !__SSE2__
#error "__SSE2__ is not defined!"
#endif

typedef long double float_t;

static float_t ldbl_min() {return 3.36210314311209350626267781732175260e-4932L;}
static float_t ldbl_denorm_min() {return 6.47517511943802511092443895822764655e-4966L;}

float_t get_smallest_value()
{
    static const float_t m = ldbl_denorm_min();
#ifdef __SSE2__
    return (__builtin_ia32_stmxcsr() & 0x8040) ? ldbl_min() : m;
#else
    return ((ldbl_min() / 2) == 0) ? ldbl_min() : m;
#endif
}

float_t foo(float_t val)
{
    return val + get_smallest_value();
}
