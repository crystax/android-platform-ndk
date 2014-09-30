#include "common.h"

using ::crystax::fileio::absolutize;

int test_absolutize()
{
    char *tmp;
    bool result;

#ifdef TEST_ABSOLUTIZE
#undef TEST_ABSOLUTIZE
#endif
#define TEST_ABSOLUTIZE(a, b) \
    tmp = absolutize(a); \
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
                "FAIL at %s:%d: absolute path is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, tmp, b); \
            return 1; \
        } \
    } \
    ::printf("ok %d - absolutize\n", __LINE__ - start);

    int start = __LINE__;
    TEST_ABSOLUTIZE(NULL, NULL);
    TEST_ABSOLUTIZE("", NULL);
    TEST_ABSOLUTIZE("a", "/a");
    TEST_ABSOLUTIZE("/a", "/a");
    TEST_ABSOLUTIZE("a/b/c", "/a/b/c");
    TEST_ABSOLUTIZE("/a/b/c", "/a/b/c");
    TEST_ABSOLUTIZE("a/", "/a");
    TEST_ABSOLUTIZE("/a/", "/a");
    TEST_ABSOLUTIZE("a/b/c/", "/a/b/c");
    TEST_ABSOLUTIZE("/a/b/c/", "/a/b/c");
    TEST_ABSOLUTIZE("a//", "/a");
    TEST_ABSOLUTIZE("a///", "/a");
    TEST_ABSOLUTIZE("//a", "/a");
    TEST_ABSOLUTIZE("///a", "/a");
    TEST_ABSOLUTIZE("//a//", "/a");
    TEST_ABSOLUTIZE("a//b//c", "/a/b/c");
    TEST_ABSOLUTIZE("//a//b//c", "/a/b/c");
    TEST_ABSOLUTIZE("a////////b//////c/////////", "/a/b/c");
    TEST_ABSOLUTIZE("///////a/////////b///////////c/////////", "/a/b/c");
    TEST_ABSOLUTIZE(".", "/");
    TEST_ABSOLUTIZE("./", "/");
    TEST_ABSOLUTIZE("/.", "/");
    TEST_ABSOLUTIZE("/./", "/");
    TEST_ABSOLUTIZE("./a", "/a");
    TEST_ABSOLUTIZE("a/.", "/a");
    TEST_ABSOLUTIZE("/./a", "/a");
    TEST_ABSOLUTIZE("/a/.", "/a");
    TEST_ABSOLUTIZE("/./a/", "/a");
    TEST_ABSOLUTIZE("/a/./", "/a");
    TEST_ABSOLUTIZE("./././././a/./././", "/a");
    TEST_ABSOLUTIZE("/./././a/.////./", "/a");
    TEST_ABSOLUTIZE("..", "/");
    TEST_ABSOLUTIZE("../", "/");
    TEST_ABSOLUTIZE("/..", "/");
    TEST_ABSOLUTIZE("/../", "/");
    TEST_ABSOLUTIZE("../a", "/a");
    TEST_ABSOLUTIZE("../a/", "/a");
    TEST_ABSOLUTIZE("/../a", "/a");
    TEST_ABSOLUTIZE("/../a/", "/a");
    TEST_ABSOLUTIZE("a/..", "/");
    TEST_ABSOLUTIZE("a/../", "/");
    TEST_ABSOLUTIZE("/a/..", "/");
    TEST_ABSOLUTIZE("/a/../", "/");
    TEST_ABSOLUTIZE("../../../../a", "/a");
    TEST_ABSOLUTIZE("../../../../a/", "/a");
    TEST_ABSOLUTIZE("../../../../a/..", "/");
    TEST_ABSOLUTIZE("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_ABSOLUTIZE("..////./././../../aaa/bb/./..////", "/aaa");
    TEST_ABSOLUTIZE("////./././../..../......//a/b/c/../d//", "/..../....../a/b/d");

#undef TEST_ABSOLUTIZE

    ::printf("ok\n");

    return 0;
}
