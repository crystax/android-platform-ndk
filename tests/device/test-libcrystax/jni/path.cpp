#include "common.h"
#include <crystax/path.hpp>

using ::crystax::fileio::path_t;
using ::crystax::fileio::abspath_t;

int test_path()
{
#ifdef TEST_PATH
#undef TEST_PATH
#endif
#ifdef TEST_ABSPATH
#undef TEST_ABSPATH
#endif

#define TEST_PATH(a, b) \
    { \
        path_t p(a); \
        bool ok = !b ? !p : p && ::strcmp(p.c_str(), b) == 0; \
        if (!ok) \
        { \
            ::fprintf(stderr, \
                "FAIL at %s:%d: path is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, p.c_str(), b); \
            return 1; \
        } \
        ::printf("ok %d - path\n", __LINE__ - start); \
    }

#define TEST_ABSPATH(a, b) \
    { \
        abspath_t p(a); \
        bool ok = !b ? !p : p &&::strcmp(p.c_str(), b) == 0; \
        if (!ok) \
        { \
            ::fprintf(stderr, \
                "FAIL at %s:%d: path is \"%s\", but expected \"%s\"\n", \
                __FILE__, __LINE__, p.c_str(), b); \
            return 1; \
        } \
        ::printf("ok %d - abspath\n", __LINE__ - start); \
    }

    int start = __LINE__;
    TEST_PATH(NULL, NULL);
    TEST_PATH("", "");
    TEST_PATH("a", "a");
    TEST_PATH("/a", "/a");
    TEST_PATH("a/b/c", "a/b/c");
    TEST_PATH("/a/b/c", "/a/b/c");
    TEST_PATH("a/", "a");
    TEST_PATH("/a/", "/a");
    TEST_PATH("a///", "a");
    TEST_PATH("a////////b//////c/////////", "a/b/c");
    TEST_PATH("./././././a/./././", "a");
    TEST_PATH("../../../../a", "../../../../a");
    TEST_PATH("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_PATH("..////./././../../aaa/bb/./..////", "../../../aaa");
    TEST_PATH("././//./../..../......//a/b/c/../d//", "../..../....../a/b/d");
    TEST_ABSPATH(NULL, NULL);
    TEST_ABSPATH("", NULL);
    TEST_ABSPATH("a", "/a");
    TEST_ABSPATH("/a", "/a");
    TEST_ABSPATH("a/b/c", "/a/b/c");
    TEST_ABSPATH("/a/b/c", "/a/b/c");
    TEST_ABSPATH("a/", "/a");
    TEST_ABSPATH("/a/", "/a");
    TEST_ABSPATH("a///", "/a");
    TEST_ABSPATH("a////////b//////c/////////", "/a/b/c");
    TEST_ABSPATH("./././././a/./././", "/a");
    TEST_ABSPATH("../../../../a", "/a");
    TEST_ABSPATH("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_ABSPATH("..////./././../../aaa/bb/./..////", "/aaa");
    TEST_ABSPATH("././//./../..../......//a/b/c/../d//", "/..../....../a/b/d");

#if 0
    TEST_NORMALIZE("/../../../../a/../../b/c/d/../../../../../..", "/");
    TEST_NORMALIZE("..////./././../../aaa/bb/./..////", "../../../aaa");
    TEST_NORMALIZE("////./././../..../......//a/b/c/../d//", "/..../....../a/b/d");
#endif

#undef TEST_PATH
#undef TEST_ABSPATH

    ::printf("ok\n");

    return 0;
}
