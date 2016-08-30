#include <cstddef>
#include <cstdlib>
#include <iostream>

template <typename T>
struct padding
{
    char begin;
    T object;
};

template <typename T>
void test(const char *tname)
{
    padding<T> p;
    std::size_t s1 = __alignof(T);
    std::size_t s2 = (char*)&p.object - &p.begin;
    if (s1 == s2) return;
    std::cerr << "ERROR (" << tname << "): " << s1 << " != " << s2 << "\n";
    std::abort();
}

#define TEST(type) test<type>(#type)

#if defined(__clang__)
#define CLANG_VERSION_GREATER_THAN(major, minor) \
    (__clang_major__ > major || (__clang_major__ == major && __clang_minor__ > minor))
#else
#define CLANG_VERSION_GREATER_THAN(major, minor) 0
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define GCC_VERSION_GREATER_THAN(major, minor) \
    (__GNUC__ > major || (__GNUC__ == major && __GNUC_MINOR__ > minor))
#else
#define GCC_VERSION_GREATER_THAN(major, minor) 0
#endif

int main()
{
    // These tests failed for 32-bit x86 target being built by both gcc and clang
    // This is not Android-specific; both gcc and clang behave the same on 32-bit
    // linux and darwin targets. However, this is obviously a bug, so this test
    // here to track it. Right now, we know gcc-4.9, gcc-5.3, gcc-6.1, clang-3.6
    // clang-3.7 and clang-3.8 are failing. In the future, this bug should be fixed, so next
    // gcc/clang versions should pass.

#if !defined(__i386__) || CLANG_VERSION_GREATER_THAN(3, 8) || GCC_VERSION_GREATER_THAN(6, 2)
    TEST(long long);
    TEST(double);
    TEST(long double);
#endif

    std::cout << "OK\n";

    return 0;
}
