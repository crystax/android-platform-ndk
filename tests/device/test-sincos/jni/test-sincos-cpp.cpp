#include <math.h>

extern "C"
float test_sincos_cpp(float a)
{
    return sin(a) + cos(a);
}
