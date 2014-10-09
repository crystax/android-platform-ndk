#include "common.h"

#include <string>

#define TEST_RESULT(type, got, exp)                        \
    if (got != exp) {                                      \
        std::cout << "to_string(" #type ") failed: got: "  \
                  << got                                   \
                  << "; expected: "                        \
                  << exp                                   \
                  << std::endl;                            \
        return 1;                                          \
    }


int test_to_string()
{
    std::string s1 = std::to_string(33);
    std::string s1_exp = "33";
    TEST_RESULT(int, s1, s1_exp);

    std::string s2 = std::to_string(2105238571L);
    std::string s2_exp = "2105238571";
    TEST_RESULT(long, s2, s2_exp);

    std::string s3 = std::to_string(4611686018427387903LL);
    std::string s3_exp = "4611686018427387903";
    TEST_RESULT(long long, s3, s3_exp);

    std::string s4 = std::to_string(77U);
    std::string s4_exp = "77";
    TEST_RESULT(unsigned, s4, s4_exp);

    std::string s5 = std::to_string(2105238571UL);
    std::string s5_exp = "2105238571";
    TEST_RESULT(unsigned long, s5, s5_exp);

    std::string s6 = std::to_string(9223372036854775806ULL);
    std::string s6_exp = "9223372036854775806";
    TEST_RESULT(long long, s6, s6_exp);

    std::string s7 = std::to_string(float(1965.0));
    std::string s7_exp = "1965.000000";
    TEST_RESULT(float, s7, s7_exp);

    double v8 = 1965.05;
    // std::string s8 = std::to_string(double(1965.05));
    std::string s8 = std::to_string(v8);
    std::string s8_exp = "1965.050000";
    TEST_RESULT(double, s8, s8_exp);

    long double v9 = 1965.05;
    std::string s9 = std::to_string(v9);
    std::string s9_exp = "1965.050000";
    TEST_RESULT(long double, s9, s9_exp);

    // printf("printf (%%f): %f\n", v9);
    // printf("printf (%%e): %e\n", v9);
    // printf("printf (%%Lf): %Lf\n", v9);
    // printf("printf (%%Le): %Le\n", v9);

    // char buf[2048];
    // memset(buf, 0, sizeof(buf));
    // int num1 = std::sprintf(buf, "%Lf", v9);
    // printf("sprintf (%%Lf): %s\nnum chars: %d\n", buf, num1);
    // memset(buf, 0, sizeof(buf));
    // int num2 = std::sprintf(buf, "%Le", v9);
    // printf("sprintf (%%Le): %s\nnum chars: %d\n", buf, num2);

    return 0;
}
