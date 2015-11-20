#if __gnu_linux__
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "foo.h"

int test_dladdr_from_exe()
{
    return MAGIC_NUMBER_FROM_EXE;
}

int main()
{
    void *p;
    int (*f)();
    Dl_info info;

    p = dlopen("libfoo1696311d025b40ef9b7e50f339ae857d.so", RTLD_NOW);
    assert(p != NULL);

    f = (int (*)())dlsym(p, "test_dladdr");
    assert(f != NULL);

    memset(&info, 0, sizeof(info));
    assert(dladdr(f, &info) != 0);
    assert(info.dli_fname != NULL);
    assert(info.dli_fbase != NULL);
    assert(info.dli_sname != NULL);
    assert(info.dli_saddr != NULL);

    assert(f() == MAGIC_NUMBER_FROM_SO);

    dlclose(p);
    return 0;
}
