#include <stdio.h>
#include <crystax.h>

int main(void)
{
    if (crystax_jvm() != NULL) {
        printf("ERROR: crystax_jvm() returned not NULL pointer!\n");
        return 1;
    }

    return 0;
}
