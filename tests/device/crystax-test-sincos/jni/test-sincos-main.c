#include <stdio.h>

extern float test_sincos_c();
extern float test_sincos_cpp();

int main()
{
    float a, b;

    printf("test-sincos - begin\n");

    a = test_sincos_c(0.3);
    b = test_sincos_cpp(0.3);
    if (a != b) {
        fprintf(stderr, "FAILED: a=%f, b=%f\n", a, b);
        return 1;
    }

    printf("OK\n");
    return 0;
}
