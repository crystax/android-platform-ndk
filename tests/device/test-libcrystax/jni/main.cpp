#include "common.h"

int main()
{
#ifdef DO_TEST
#undef DO_TEST
#endif
#define DO_TEST(name) if (test_ ## name () != 0) return 1

    DO_TEST(is_normalized);
    DO_TEST(normalize);
    DO_TEST(is_absolute);
    DO_TEST(absolutize);
    DO_TEST(is_subpath);
    DO_TEST(relpath);
    DO_TEST(basename);
    DO_TEST(dirname);
    DO_TEST(path);
    DO_TEST(list);
    DO_TEST(open_self);

#undef DO_TEST
    return 0;
}
