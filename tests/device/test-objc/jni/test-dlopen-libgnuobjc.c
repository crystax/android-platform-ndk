#include <stdio.h>
#include <dlfcn.h>

int main()
{
    const char *plib = "/data/local/tmp/libgnuobjc_shared.so";

    if (!dlopen(plib, RTLD_NOW)) {
        char *err = dlerror();
        printf("error dlopen %s: %s\n", plib, err);
        return 1;
    }

    return 0;
}
