#include <common.h>

GLOBAL
int test_swprintf()
{
    wchar_t wbuf[1024];

#define DO_WPRINTF_TEST(n, check, fmt, ...) \
    if (swprintf(wbuf, sizeof wbuf, L##fmt, ##__VA_ARGS__) != wcslen(L##check)) \
    { \
        printf("FAIL! wbuf=\"%ls\", check=\"%ls\"\n", wbuf, L##check); \
        return 1; \
    } \
    if (wcscmp(wbuf, L##check) != 0) \
    { \
        printf("FAIL! wbuf=\"%ls\", check=\"%ls\"\n", wbuf, L##check); \
        return 1; \
    } \
    printf("ok " #n " - swprintf\n")

#include "test-wprintf-local.c"

#undef DO_WPRINTF_TEST

    return 0;
}

GLOBAL
int test_fwprintf()
{
    int ret;
    int len;
#define DO_WPRINTF_TEST(n, check, fmt, ...) \
    ret = fwprintf(stderr, L##fmt "\n", ##__VA_ARGS__); \
    if (ret < 0) \
    { \
        printf("FAIL! fwprintf return %d\n", ret); \
        return 1; \
    } \
    len = wcslen(L##check) + 1; \
    if (ret != len) \
    { \
        printf("FAIL! fwprintf return %d, but \"%ls\" is %d-byte long\n", ret, L##check, len); \
        return 1; \
    } \
    printf("ok " #n " - fwprintf\n")

#include "test-wprintf-local.c"

#undef DO_WPRINTF_TEST

    return 0;
}

GLOBAL
int test_wprintf()
{
    int ret;
    int len;
#define DO_WPRINTF_TEST(n, check, fmt, ...) \
    ret = wprintf(L##fmt "\n", ##__VA_ARGS__); \
    if (ret < 0) \
    { \
        printf("FAIL! wprintf return %d\n", ret); \
        return 1; \
    } \
    len = wcslen(L##check) + 1; \
    if (ret != len) \
    { \
        printf("FAIL! wprintf return %d, but \"%ls\" is %d-byte long\n", ret, L##check, len); \
        return 1; \
    } \
    printf("ok " #n " - wprintf\n")

#include "test-wprintf-local.c"

#undef DO_WPRINTF_TEST

    return 0;
}

GLOBAL
int test_wprintf_all()
{
#define DO_WPRINTF_TEST1(x) if (test_ ## x ()) return 1
    DO_WPRINTF_TEST1(swprintf);
    DO_WPRINTF_TEST1(fwprintf);
    DO_WPRINTF_TEST1(wprintf);
#undef DO_WPRINTF_TEST1
    return 0;
}

#ifndef __ANDROID__
int main()
{
    return test_wprintf_all();
}
#endif
