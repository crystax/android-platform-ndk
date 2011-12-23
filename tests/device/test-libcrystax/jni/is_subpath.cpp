#include "common.h"

using ::crystax::fileio::is_subpath;

int test_is_subpath()
{
#ifndef TEST_SUBPATH
#undef TEST_SUBPATH
#endif
#ifndef TEST_NOT_SUBPATH
#undef TEST_NOT_SUBPATH
#endif

#define TEST_SUBPATH(root, path) \
    if (!is_subpath(root, path)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is not subpath of \"%s\"\n", \
            __FILE__, __LINE__, path, root); \
        return 1; \
    } \
    ::printf("ok %d - is_subpath\n", __LINE__ - start)
#define TEST_NOT_SUBPATH(root, path) \
    if (is_subpath(root, path)) \
    { \
        ::fprintf(stderr, \
            "FAIL at %s:%d: path \"%s\" is subpath of \"%s\"\n", \
            __FILE__, __LINE__, path, root); \
        return 1; \
    } \
    ::printf("ok %d - !is_subpath\n", __LINE__ - start)

    int start = __LINE__;
    TEST_NOT_SUBPATH(NULL, NULL);
    TEST_NOT_SUBPATH(NULL, "");
    TEST_NOT_SUBPATH("", NULL);
    TEST_NOT_SUBPATH("", "");
    TEST_NOT_SUBPATH("/", "");
    TEST_NOT_SUBPATH(".", "");
    TEST_SUBPATH("/", "/a");
    TEST_SUBPATH("/", "a");
    TEST_SUBPATH(".", "/a");
    TEST_SUBPATH(".", "a");
    TEST_SUBPATH(".", "./a");
    TEST_SUBPATH("/a", "/a");
    TEST_SUBPATH("/a", "/a/b/c");
    TEST_NOT_SUBPATH("/a", "/");
    TEST_NOT_SUBPATH("/a/b/c", "/a");
    TEST_NOT_SUBPATH("/a", "/b");
    TEST_NOT_SUBPATH("/a/b/c", "/a/d/e");
    TEST_NOT_SUBPATH("/a/b", "/a/bb");

#undef TEST_SUBPATH
#undef TEST_NOT_SUBPATH

    printf("ok\n");

    return 0;
}
