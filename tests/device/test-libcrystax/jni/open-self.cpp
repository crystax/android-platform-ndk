#include "common.h"

int test_open_self()
{
    const char *s = "/proc/self/cmdline";
    FILE* fp = fopen(s, "rb");
    if (!fp)
    {
        ::fprintf(stderr, \
            "FAIL at %s:%d: can't open \"%s\"\n", \
            __FILE__, __LINE__, s);
        return 1;
    }

    printf("ok\n");
    return 0;
}
