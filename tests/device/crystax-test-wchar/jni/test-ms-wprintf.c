#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE  1024
#define ENV_VAR   "CRYSTAX_USE_MS_STYLE_WPRINTF"

int main()
{
    wchar_t wbuf[BUF_SIZE];
    int len;
    int wbuflen;

    /* GCC style [s]wprintf */

    if (unsetenv(ENV_VAR) != 0) {
        printf("FAIL! unsetenv failed with errno: %d; error: %s\n", errno, strerror(errno));
        return 1;
    }

    memset(wbuf, 0, sizeof wbuf);
    len = swprintf(wbuf, BUF_SIZE, L"%s", "abcd");
    wbuflen = wcslen(wbuf);
    printf("len: %d; wbuflen: %d; wbuf: %ls\n", len, wbuflen, wbuf);
    if (len < 0) {
        printf("FAIL! (GCC %%s) len: %d; errno: %d; error: %s\n", len, errno, strerror(errno));
        return 1;
    }
    if (len != wbuflen) {
        printf("FAIL! (GCC %%s) len: %d; wbuflen: %d\n", len, wbuflen);
        return 1;
    }

    memset(wbuf, 0, sizeof wbuf);
    len = swprintf(wbuf, BUF_SIZE, L"%c", 'a');
    wbuflen = wcslen(wbuf);
    printf("len: %d; wbuflen: %d; wbuf: %ls\n", len, wbuflen, wbuf);
    if (len < 0) {
        printf("FAIL! (GCC %%c) len: %d; errno: %d; error: %s\n", len, errno, strerror(errno));
        return 1;
    }
    if (len != wbuflen) {
        printf("FAIL! (GCC %%c) len: %d; wbuflen: %d\n", len, wbuflen);
        return 1;
    }

    /* MS style [s]wprintf */

    if (setenv("CRYSTAX_USE_MS_STYLE_WPRINTF", "yes", 1) != 0) {
        printf("FAIL! setenv failed with errno: %d; error: %s\n", errno, strerror(errno));
        return 1;
    }

    memset(wbuf, 0, sizeof wbuf);
    len = swprintf(wbuf, BUF_SIZE, L"%s", L"abcd");
    wbuflen = wcslen(wbuf);
    printf("len: %d; wbuflen: %d; wbuf: %ls\n", len, wbuflen, wbuf);
    if (len < 0) {
        printf("FAIL! (MS %%s) len: %d; errno: %d; error: %s\n", len, errno, strerror(errno));
        return 1;
    }
    if (len != wbuflen) {
        printf("FAIL! (GCC %%s) len: %d; wbuflen: %d\n", len, wbuflen);
        return 1;
    }

    memset(wbuf, 0, sizeof wbuf);
    len = swprintf(wbuf, BUF_SIZE, L"%c", L'a');
    wbuflen = wcslen(wbuf);
    printf("len: %d; wbuflen: %d; wbuf: %ls\n", len, wbuflen, wbuf);
    if (len < 0) {
        printf("FAIL! (MS %%c) len: %d; errno: %d; error: %s\n", len, errno, strerror(errno));
        return 1;
    }
    if (len != wbuflen) {
        printf("FAIL! (MS %%c) len: %d; wbuflen: %d\n", len, wbuflen);
        return 1;
    }


    /* clean the environment */

    if (unsetenv(ENV_VAR) != 0) {
        printf("FAIL! unsetenv failed with errno: %d; error: %s\n", errno, strerror(errno));
        return 1;
    }

    return 0;
}
