#include <typeinfo>

#if __LP64__
#define LONG_DOUBLE_SIZE 16
#else
#define LONG_DOUBLE_SIZE 8
#endif

static_assert(sizeof(long double) == LONG_DOUBLE_SIZE, "Wrong size of 'long double'");

extern "C"
const char *ldname()
{
    return typeid(long double).name();
}
