#include "common.h"

using ::crystax::fileio::relpath;

int test_relpath()
{
    char *tmp;
    bool result;

#ifdef TEST_RELPATH
#undef TEST_RELPATH
#endif
#define TEST_RELPATH(root, path, expected) \
    tmp = relpath(root, path); \
    if (tmp == NULL) \
        assert(expected == NULL); \
    else \
    { \
        assert(expected != NULL); \
        result = ::strcmp(tmp, expected) == 0; \
        ::free((void*)tmp); \
        if (!result) \
        { \
            ::fprintf(stderr, \
                "FAIL at %s:%d: relative path is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, tmp, expected); \
            return 1; \
        } \
    } \
    ::printf("ok %d - relpath\n", __LINE__ - start);

    int start = __LINE__;
    TEST_RELPATH("/", "/a", "a");
    TEST_RELPATH("/", "a", "a");
    TEST_RELPATH("/a", "/a/b/c", "b/c");
    TEST_RELPATH("/a", "/aa", NULL);
    TEST_RELPATH("a/b", "a/b/c", "c");
    TEST_RELPATH("a/b", "a/bb/c", NULL);
    TEST_RELPATH("a/../b/.///./c", "a", NULL);
    TEST_RELPATH("a/../b/.///./c", "./e//./..//b/./f///../c//d/", "d");

#undef TEST_RELPATH

    printf("ok\n");

    return 0;
}
