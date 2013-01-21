#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024


int test_printf()
{
    int ret;
    int len;

#define DO_PRINTF_TEST(n, check, fmt, ...) \
    ret = printf(fmt "\n", ##__VA_ARGS__); \
    if (ret < 0) { \
        printf("FAIL! printf return %d\n", ret); \
        return 1; \
    } \
    len = strlen(check) + 1; \
    if (ret != len) { \
        printf("FAIL! printf return %d, but \"%s\" is %d-byte long\n", ret, check, len); \
        return 1; \
    } \
    printf("printf " #n " - ok\n")

#include "test-printf-data.c"

#undef DO_PRINTF_TEST

    return 0;
}

int test_fprintf()
{
    int ret;
    int len;

#define DO_PRINTF_TEST(n, check, fmt, ...) \
    ret = fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
    if (ret < 0) { \
        printf("FAIL! fprintf return %d\n", ret); \
        return 1; \
    } \
    len = strlen(check) + 1; \
    if (ret != len) { \
        printf("FAIL! fprintf return %d, but \"%s\" is %d-byte long\n", ret, check, len); \
        return 1; \
    } \
    printf("fprintf " #n " - ok\n")

#include "test-printf-data.c"

#undef DO_PRINTF_TEST

    return 0;
}

int test_sprintf()
{
    char buf[BUF_SIZE];
    int len, ret;

#define DO_PRINTF_TEST(n, check, fmt, ...) \
    memset(buf, 0, sizeof buf); \
    ret = sprintf(buf, fmt, ##__VA_ARGS__); \
    len = strlen(check); \
    if (len != ret) { \
        printf("FAIL! buf len is %d, but expected %d\n", ret, len); \
        return 1; \
    } \
    if (strcmp(buf, check) != 0) { \
        printf("FAIL! buf is \"%s\", but expected \"%s\"\n", buf, check); \
        return 1; \
    } \
    printf("sprintf " #n " - ok\n")

#include "test-printf-data.c"

#undef DO_PRINTF_TEST

    return 0;
}

int test_snprintf()
{
    char buf[BUF_SIZE];
    int len, ret;

#define DO_PRINTF_TEST(n, check, fmt, ...) \
    memset(buf, 0, sizeof buf); \
    ret = snprintf(buf, BUF_SIZE, fmt, ##__VA_ARGS__);   \
    len = strlen(check); \
    if (len != ret) { \
        printf("FAIL! buf len is %d, but expected %d\n", ret, len); \
        return 1; \
    } \
    if (strcmp(buf, check) != 0) { \
        printf("FAIL! buf is \"%s\", but expected \"%s\"\n", buf, check); \
        return 1; \
    } \
    printf("snprintf " #n " - ok\n")

#include "test-printf-data.c"

#undef DO_PRINTF_TEST

    return 0;
}
