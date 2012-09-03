#include "common.h"

using ::crystax::fileio::basename;

int test_basename()
{
    char *tmp;
    bool result;

#ifdef TEST_BASENAME
#undef TEST_BASENAME
#endif
#define TEST_BASENAME(a, b) \
    tmp = basename(a); \
    if (tmp == NULL) \
        assert(b == NULL); \
    else \
    { \
        assert(b != NULL); \
        result = ::strcmp(tmp, b) == 0; \
        ::free((void*)tmp); \
        if (!result) \
        { \
            ::fprintf(stderr, \
                "FAIL at %s:%d: basename is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, tmp, b); \
            return 1; \
        } \
    } \
    ::printf("ok %d - basename\n", __LINE__ - start);

    int start = __LINE__;
    TEST_BASENAME(NULL, NULL);
    TEST_BASENAME("", "");
    TEST_BASENAME("a", "a");
    TEST_BASENAME("/a", "a");
    TEST_BASENAME("a/b/c", "c");
    TEST_BASENAME("/a/b/c", "c");
    TEST_BASENAME("a/", "a");
    TEST_BASENAME("/a/", "a");
    TEST_BASENAME("a/b/c/", "c");
    TEST_BASENAME("/a/b/c/", "c");
    TEST_BASENAME("a//", "a");
    TEST_BASENAME("a///", "a");
    TEST_BASENAME("//a", "a");
    TEST_BASENAME("///a", "a");
    TEST_BASENAME("//a//", "a");
    TEST_BASENAME("a//b//c", "c");
    TEST_BASENAME("//a//b//c", "c");
    TEST_BASENAME("a////////b//////c/////////", "c");
    TEST_BASENAME("///////a/////////b///////////c/////////", "c");
    TEST_BASENAME(".", ".");
    TEST_BASENAME("./", ".");
    TEST_BASENAME("/.", ".");
    TEST_BASENAME("/./", ".");
    TEST_BASENAME("./a", "a");
    TEST_BASENAME("a/.", "a");
    TEST_BASENAME("/./a", "a");
    TEST_BASENAME("/a/.", "a");
    TEST_BASENAME("/./a/", "a");
    TEST_BASENAME("/a/./", "a");
    TEST_BASENAME("./././././a/./././", "a");
    TEST_BASENAME("/./././a/.////./", "a");
    TEST_BASENAME("..", "..");
    TEST_BASENAME("../", "..");
    TEST_BASENAME("/..", ".");
    TEST_BASENAME("/../", ".");
    TEST_BASENAME("../a", "a");
    TEST_BASENAME("../a/", "a");
    TEST_BASENAME("/../a", "a");
    TEST_BASENAME("/../a/", "a");
    TEST_BASENAME("a/..", ".");
    TEST_BASENAME("a/../", ".");
    TEST_BASENAME("/a/..", ".");
    TEST_BASENAME("/a/../", ".");
    TEST_BASENAME("../../../../a", "a");
    TEST_BASENAME("../../../../a/", "a");
    TEST_BASENAME("../../../../a/..", "..");
    TEST_BASENAME("/../../../../a/../../b/c/d/../../../../../..", ".");
    TEST_BASENAME("..////./././../../aaa/bb/./..////", "aaa");
    TEST_BASENAME("////./././../..../......//a/b/c/../d//", "d");

#undef TEST_BASENAME

    printf("ok\n");

    return 0;
}
