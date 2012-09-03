#include "common.h"

using ::crystax::fileio::is_absolute;

int test_is_absolute()
{
#ifndef TEST_ABSOLUTE
#undef TEST_ABSOLUTE
#endif
#ifndef TEST_NOT_ABSOLUTE
#undef TEST_NOT_ABSOLUTE
#endif

#define TEST_ABSOLUTE(a) \
    if (!is_absolute(a)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is not absolute\n", \
            __FILE__, __LINE__, a); \
        return 1; \
    } \
    ::printf("ok %d - is_absolute\n", __LINE__ - start)
#define TEST_NOT_ABSOLUTE(a) \
    if (is_absolute(a)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is absolute\n", \
            __FILE__, __LINE__, a); \
        return 1; \
    } \
    ::printf("ok %d - !is_absolute\n", __LINE__ - start)

    int start = __LINE__;
    TEST_NOT_ABSOLUTE(NULL);
    TEST_NOT_ABSOLUTE("");
    TEST_NOT_ABSOLUTE("a");
    TEST_NOT_ABSOLUTE("a/b/c");
    TEST_NOT_ABSOLUTE("a/b/c/");
    TEST_NOT_ABSOLUTE("./a");
    TEST_NOT_ABSOLUTE("../a");
    TEST_NOT_ABSOLUTE("../a/");
    TEST_ABSOLUTE("/");
    TEST_ABSOLUTE("/a/b/c");
    TEST_ABSOLUTE("/a/b/c/");
    TEST_ABSOLUTE("/a///b/c");
    TEST_ABSOLUTE("/a/..//../.././././///a");

#undef TEST_ABSOLUTE
#undef TEST_NOT_ABSOLUTE

    ::printf("ok\n");

    return 0;
}
