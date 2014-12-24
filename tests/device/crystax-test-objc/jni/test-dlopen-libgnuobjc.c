#include <stdio.h>
#include <dlfcn.h>

int main()
{
    const char *plib = "libgnuobjc_shared.so";

    if (!dlopen(plib, RTLD_NOW)) {
        char *err = dlerror();
        printf("dlopen(\"%s\") FAILED: %s\n", plib, err);
        return 1;
    }

    printf("dlopen(\"%s\"): OK\n", plib);

    return 0;
}
