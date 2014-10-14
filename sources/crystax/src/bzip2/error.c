#include <stdio.h>
#include <stdlib.h>

void bz_internal_error(int err)
{
    fprintf(stderr, "BZip2 internal error: %d\n", err);
    abort();
}
