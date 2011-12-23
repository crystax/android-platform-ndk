#include "common.h"

using ::crystax::fileio::normalize;

int test_normalize()
{
    char *tmp;
    bool result;

#ifdef TEST_NORMALIZE
#undef TEST_NORMALIZE
#endif
#define TEST_NORMALIZE(a, b) \
    tmp = normalize(a); \
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
                "FAIL at %s:%d: normalized path is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, tmp, b); \
            return 1; \
        } \
    } \
    ::printf("ok %d - normalize\n", __LINE__ - start);

    int start = __LINE__;
    TEST_NORMALIZE(NULL, NULL);
    TEST_NORMALIZE("", "");
    TEST_NORMALIZE("a", "a");
    TEST_NORMALIZE("/a", "/a");
    TEST_NORMALIZE("a/b/c", "a/b/c");
    TEST_NORMALIZE("/a/b/c", "/a/b/c");
    TEST_NORMALIZE("a/", "a");
    TEST_NORMALIZE("/a/", "/a");
    TEST_NORMALIZE("a/b/c/", "a/b/c");
    TEST_NORMALIZE("/a/b/c/", "/a/b/c");
    TEST_NORMALIZE("a//", "a");
    TEST_NORMALIZE("a///", "a");
    TEST_NORMALIZE("//a", "/a");
    TEST_NORMALIZE("///a", "/a");
    TEST_NORMALIZE("//a//", "/a");
    TEST_NORMALIZE("a//b//c", "a/b/c");
    TEST_NORMALIZE("//a//b//c", "/a/b/c");
    TEST_NORMALIZE("a////////b//////c/////////", "a/b/c");
    TEST_NORMALIZE("///////a/////////b///////////c/////////", "/a/b/c");
    TEST_NORMALIZE(".", ".");
    TEST_NORMALIZE("./", ".");
    TEST_NORMALIZE("/.", "/");
    TEST_NORMALIZE("/./", "/");
    TEST_NORMALIZE("./a", "a");
    TEST_NORMALIZE("a/.", "a");
    TEST_NORMALIZE("/./a", "/a");
    TEST_NORMALIZE("/a/.", "/a");
    TEST_NORMALIZE("/./a/", "/a");
    TEST_NORMALIZE("/a/./", "/a");
    TEST_NORMALIZE("./././././a/./././", "a");
    TEST_NORMALIZE("/./././a/.////./", "/a");
    TEST_NORMALIZE("..", "..");
    TEST_NORMALIZE("../", "..");
    TEST_NORMALIZE("/..", "/");
    TEST_NORMALIZE("/../", "/");
    TEST_NORMALIZE("../a", "../a");
    TEST_NORMALIZE("../a/", "../a");
    TEST_NORMALIZE("/../a", "/a");
    TEST_NORMALIZE("/../a/", "/a");
    TEST_NORMALIZE("a/..", ".");
    TEST_NORMALIZE("a/../", ".");
    TEST_NORMALIZE("/a/..", "/");
    TEST_NORMALIZE("/a/../", "/");
    TEST_NORMALIZE("../../../../a", "../../../../a");
    TEST_NORMALIZE("../../../../a/", "../../../../a");
    TEST_NORMALIZE("../../../../a/..", "../../../..");
    TEST_NORMALIZE("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_NORMALIZE("..////./././../../aaa/bb/./..////", "../../../aaa");
    TEST_NORMALIZE("////./././../..../......//a/b/c/../d//", "/..../....../a/b/d");
    TEST_NORMALIZE("././//./../..../......//a/b/c/../d//", "../..../....../a/b/d");

#undef TEST_NORMALIZE

    ::printf("ok\n");

    return 0;
}
