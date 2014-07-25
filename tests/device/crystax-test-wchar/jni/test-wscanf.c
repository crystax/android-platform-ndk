#include <common.h>

GLOBAL
int test_swscanf()
{
    int ret;
    int n;

    printf("1..50\n");

#define DO_WSCANF_TEST(n, check, str, fmt, ...) \
    ret = swscanf(L##str, L##fmt, ##__VA_ARGS__); \
    if (ret != check) \
    { \
        printf("FAIL! swscanf returned %d, but expected %d\n", ret, check); \
        return 1; \
    } \
    printf("ok " #n " - swscanf\n")

    DO_WSCANF_TEST(1,  1, "100", "%d", &n);
    if (n != 100)
        return 1;

#undef DO_WSCANF_TEST

    return 0;
}

GLOBAL
int test_wscanf_all()
{
    return test_swscanf();
}

#ifndef __ANDROID__
int main()
{
    return test_wscanf_all();
}
#endif
