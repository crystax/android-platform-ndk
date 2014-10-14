#include <dlfcn.h>

void * __crystax_bionic_symbol(const char *name);

static int initialized = 0;
static int (*bionic_dladdr)(const void *addr, Dl_info *info) = 0;

int dladdr(const void *addr, Dl_info *info)
{
    if (!initialized)
    {
        bionic_dladdr = __crystax_bionic_symbol("dladdr");
        initialized = 1;
    }

    return bionic_dladdr ? bionic_dladdr(addr, info) : 0;
}
