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

int main()
{
    // These tests failed for 32-bit x86 target being built by both gcc and clang
    // This is not Android-specific; both gcc and clang behave the same on 32-bit
    // linux and darwin targets. However, this is obviously a bug, so this test
    // here to track it. Right now, we know gcc-4.9, gcc-5.1 and clang-3.6 are failing.
    // In the future, this bug should be fixed, so next gcc/clang versions should
    // pass.

#if !(defined(__i386__) && ( \
    (defined(__clang__) && (__clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ <= 6))) || \
    (defined(__GNUC__) && (__GNUC__ < 5 || (__GNUC__ == 5 && __GNUC_MINOR__ <= 2))) \
    ))
    TEST(long long);
    TEST(double);
    TEST(long double);
#endif

    std::cout << "OK\n";

    return 0;
}
