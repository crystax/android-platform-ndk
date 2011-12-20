#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "android.h"

int
_swrite(FILE *fp, char const *buf, int n)
{
    int i, ret;

    if (n < 0)
    {
        errno = EFAULT;
        return -1;
    }

    for (i = 0; i < n; ++i)
    {
        ret = __sputc(buf[i], fp);
        if (ret < 0)
            return ret;
    }
    return 0;
}
