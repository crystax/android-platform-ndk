#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
    assert(sizeof(double) == sizeof(long double));

    double d = strtod("12.345678", NULL);
    long double ld = strtold("12.345678", NULL);

    /* printf("d = %f\n", d); */
    /* printf("ld = %lf\n", ld); */

    if (d != ld) {
        printf("error: strtold produces unexptected result: %Lf\n", ld);
        return 1;
    }

    _Exit(0);

    printf("error: _Exit() function does not work\n");
    return 1;
}
