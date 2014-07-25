#include "common.h"


#if !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error __GXX_EXPERIMENTAL_CXX0X__ undefined!
#endif

#if !defined(_GLIBCXX_USE_C99)
#error _GLIBCXX_USE_C99 undefined!
#endif


#include <string>

#define TEST_RESULT(type, got, exp)                          \
    if (got != exp) {                                        \
        std::wcout << "to_wstring(" #type ") failed: got: "  \
                   << got                                    \
                   << "; expected: "                         \
                   << exp                                    \
                   << std::endl;                             \
        return 1;                                            \
    }

int test_to_wstring()
{
    std::wstring s1 = std::to_wstring(33);
    std::wstring s1_exp = L"33";
    TEST_RESULT(int, s1, s1_exp);

    std::wstring s2 = std::to_wstring(2105238571L);
    std::wstring s2_exp = L"2105238571";
    TEST_RESULT(long, s2, s2_exp);

    std::wstring s3 = std::to_wstring(4611686018427387903LL);
    std::wstring s3_exp = L"4611686018427387903";
    TEST_RESULT(long long, s3, s3_exp);

    std::wstring s4 = std::to_wstring(77U);
    std::wstring s4_exp = L"77";
    TEST_RESULT(unsigned, s4, s4_exp);

    std::wstring s5 = std::to_wstring(2105238571UL);
    std::wstring s5_exp = L"2105238571";
    TEST_RESULT(unsigned long, s5, s5_exp);

    std::wstring s6 = std::to_wstring(9223372036854775806ULL);
    std::wstring s6_exp = L"9223372036854775806";
    TEST_RESULT(unsigned long long, s6, s6_exp);

    std::wstring s7 = std::to_wstring(float(1965.0));
    std::wstring s7_exp = L"1965.000000";
    TEST_RESULT(float, s7, s7_exp);

    std::wstring s8 = std::to_wstring(double(1965.05));
    std::wstring s8_exp = L"1965.050000";
    TEST_RESULT(double, s8, s8_exp);

    long double v9 = 1965.05;
    std::wstring s9 = std::to_wstring(v9);
    std::wstring s9_exp = L"1965.050000";
    TEST_RESULT(long double, s9, s9_exp);

    return 0;
}
