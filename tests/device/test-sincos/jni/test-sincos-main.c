#include <stdio.h>

extern float test_sincos_c();
extern float test_sincos_cpp();

int main()
{
    float a, b;
    a = test_sinc_c(0.3);
    b = test_sincos_cpp(0.3);
    printf("a=%f, b=%f\n", a, b);
    return 0;
}
