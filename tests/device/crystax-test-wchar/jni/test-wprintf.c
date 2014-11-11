#include <stdio.h>
#include <wchar.h>
#include <string.h>

int test_swprintf()
{

#define WBUF_SIZE 1024

    wchar_t wbuf[WBUF_SIZE];
    int len, checklen;

#define DO_WPRINTF_TEST(n, check, fmt, ...) \
    memset(wbuf, 0, sizeof wbuf); \
    len = swprintf(wbuf, WBUF_SIZE, L##fmt, ##__VA_ARGS__); \
    checklen = wcslen(L##check); \
    if (len != checklen) \
    { \
        printf("FAIL! wbuf len is %d, but expected %d\n", len, checklen); \
        return 1; \
    } \
    if (wcscmp(wbuf, L##check) != 0) \
    { \
        printf("FAIL! wbuf is \"%ls\", but expected \"%ls\"\n", wbuf, L##check); \
        return 1; \
    } \
    printf("ok " #n " - swprintf\n")

#include "wprintf.inc"

#undef DO_WPRINTF_TEST

    return 0;
}

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

#include "wprintf.inc"

#undef DO_WPRINTF_TEST

    return 0;
}

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

#include "wprintf.inc"

#undef DO_WPRINTF_TEST

    return 0;
}

int main()
{
#define DO_WPRINTF_TEST1(x) if (test_ ## x ()) return 1
    DO_WPRINTF_TEST1(swprintf);
    DO_WPRINTF_TEST1(fwprintf);
    DO_WPRINTF_TEST1(wprintf);
#undef DO_WPRINTF_TEST1
    return 0;
}
