#include <stdio.h>
#include <stdlib.h>

int main()
{
    double d;
    double expected = 4660.0;

    d = strtod("0x1234", NULL);
    if (d != expected)
    {
        fprintf(stderr, "strtod() failed: %f != %f\n", d, expected);
        return 1;
    }

    printf("strtod() OK: %f\n", d);

    return 0;
}
