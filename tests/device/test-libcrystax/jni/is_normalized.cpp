#include "common.h"

using ::crystax::fileio::is_normalized;

int test_is_normalized()
{
#ifndef TEST_NORMALIZED
#undef TEST_NORMALIZED
#endif
#ifndef TEST_NOT_NORMALIZED
#undef TEST_NOT_NORMALIZED
#endif

#define TEST_NORMALIZED(a) \
    if (!is_normalized(a)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is not normalized\n", \
            __FILE__, __LINE__, a); \
        return 1; \
    } \
    ::printf("ok %d - is_normalized\n", __LINE__ - start)
#define TEST_NOT_NORMALIZED(a) \
    if (is_normalized(a)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is normalized\n", \
            __FILE__, __LINE__, a); \
        return 1; \
    } \
    ::printf("ok %d - !is_normalized\n", __LINE__ - start)

    int start = __LINE__;
    TEST_NORMALIZED(NULL);
    TEST_NORMALIZED("");
    TEST_NORMALIZED("/");
    TEST_NORMALIZED("/a");
    TEST_NORMALIZED("a");
    TEST_NORMALIZED("/a/b/c");
    TEST_NORMALIZED("a/b/c");
    TEST_NOT_NORMALIZED("//");
    TEST_NOT_NORMALIZED("/a/");
    TEST_NOT_NORMALIZED("a/");
    TEST_NOT_NORMALIZED("/a/b/c/");
    TEST_NOT_NORMALIZED("a/b/c/");
    TEST_NOT_NORMALIZED("/a/b//c");
    TEST_NOT_NORMALIZED("a/b/c/////d");
    TEST_NOT_NORMALIZED(".");
    TEST_NOT_NORMALIZED("/./");
    TEST_NOT_NORMALIZED("/./a");
    TEST_NOT_NORMALIZED("./");
    TEST_NOT_NORMALIZED("./a");
    TEST_NOT_NORMALIZED("..");
    TEST_NOT_NORMALIZED("/..");
    TEST_NOT_NORMALIZED("../");
    TEST_NOT_NORMALIZED("/../");
    TEST_NOT_NORMALIZED("../a");
    TEST_NOT_NORMALIZED("/../a");
    TEST_NOT_NORMALIZED("..");

#undef DO_TEST

    ::printf("ok\n");

    return 0;
}
