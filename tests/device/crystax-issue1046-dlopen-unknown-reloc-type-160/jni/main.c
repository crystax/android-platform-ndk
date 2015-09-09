#include <dlfcn.h>
#include <stdio.h>

#define LIBNAME "libtestlib.so"

int main()
{
    void *pc = dlopen(LIBNAME, RTLD_NOW);
    if (!pc)
    {
        fprintf(stderr, "ERROR: Can't load %s: %s\n", LIBNAME, dlerror());
        return 1;
    }

    printf("OK\n");
    return 0;
}
