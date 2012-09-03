#include "common.h"

using ::crystax::fileio::dirname;

int test_dirname()
{
    char *tmp;
    bool result;

#ifdef TEST_DIRNAME
#undef TEST_DIRNAME
#endif
#define TEST_DIRNAME(a, b) \
    tmp = dirname(a); \
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
                "FAIL at %s:%d: dirname is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, tmp, b); \
            return 1; \
        } \
    } \
    ::printf("ok %d - dirname\n", __LINE__ - start);

    int start = __LINE__;
    TEST_DIRNAME(NULL, NULL);
    TEST_DIRNAME("", ".");
    TEST_DIRNAME("a", ".");
    TEST_DIRNAME("/a", "/");
    TEST_DIRNAME("a/b/c", "a/b");
    TEST_DIRNAME("/a/b/c", "/a/b");
    TEST_DIRNAME("a/", ".");
    TEST_DIRNAME("/a/", "/");
    TEST_DIRNAME("a/b/c/", "a/b");
    TEST_DIRNAME("/a/b/c/", "/a/b");
    TEST_DIRNAME("a//", ".");
    TEST_DIRNAME("a///", ".");
    TEST_DIRNAME("//a", "/");
    TEST_DIRNAME("///a", "/");
    TEST_DIRNAME("//a//", "/");
    TEST_DIRNAME("a//b//c", "a/b");
    TEST_DIRNAME("//a//b//c", "/a/b");
    TEST_DIRNAME("a////////b//////c/////////", "a/b");
    TEST_DIRNAME("///////a/////////b///////////c/////////", "/a/b");
    TEST_DIRNAME(".", ".");
    TEST_DIRNAME("./", ".");
    TEST_DIRNAME("/.", "/");
    TEST_DIRNAME("/./", "/");
    TEST_DIRNAME("./a", ".");
    TEST_DIRNAME("a/.", ".");
    TEST_DIRNAME("/./a", "/");
    TEST_DIRNAME("/a/.", "/");
    TEST_DIRNAME("/./a/", "/");
    TEST_DIRNAME("/a/./", "/");
    TEST_DIRNAME("./././././a/./././", ".");
    TEST_DIRNAME("/./././a/.////./", "/");
    TEST_DIRNAME("..", ".");
    TEST_DIRNAME("../", ".");
    TEST_DIRNAME("/..", "/");
    TEST_DIRNAME("/../", "/");
    TEST_DIRNAME("../a", "..");
    TEST_DIRNAME("../a/", "..");
    TEST_DIRNAME("/../a", "/");
    TEST_DIRNAME("/../a/", "/");
    TEST_DIRNAME("a/..", ".");
    TEST_DIRNAME("a/../", ".");
    TEST_DIRNAME("/a/..", "/");
    TEST_DIRNAME("/a/../", "/");
    TEST_DIRNAME("../../../../a", "../../../..");
    TEST_DIRNAME("../../../../a/", "../../../..");
    TEST_DIRNAME("../../../../a/..", "../../..");
    TEST_DIRNAME("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_DIRNAME("..////./././../../aaa/bb/./..////", "../../..");
    TEST_DIRNAME("////./././../..../......//a/b/c/../d//", "/..../....../a/b");

#undef TEST_DIRNAME

    printf("ok\n");

    return 0;
}
