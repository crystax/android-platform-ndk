#include "common.h"

#include <math.h>
#include <string>

#define EPSILON 0.0001

#define TEST_RESULT(fun, got, exp)         \
    if (got != exp) {                      \
        std::cout << fun " failed: got: "  \
                  << got                   \
                  << "; expected: "        \
                  << exp                   \
                  << std::endl;            \
        return 1;                          \
    }

#define TEST_FLOAT_RESULT(fun, abs, double_type, got, exp)             \
    {                                                                  \
      double_type diff = abs(got - exp);                               \
      if (diff > EPSILON) {                                            \
          printf("got: %e; expected: %e; diff: %e\n", got, exp, diff); \
          return 1;                                                    \
      }                                                                \
    }


int test_stol()
{
    std::string s1 = "17";
    int v1 = std::stoi(s1);
    TEST_RESULT("stoi", v1, 17);

    std::string s2 = "701746190";
    long v2 = std::stol(s2);
    TEST_RESULT("stol", v2, 701746190L);

    std::string s3 = "4611686018427387903";
    long long v3 = std::stoll(s3);
    TEST_RESULT("stol", v3, 4611686018427387903LL);

    std::string s4 = "701746190";
    unsigned long v4 = std::stoul(s4);
    TEST_RESULT("stoul", v4, 701746190UL);

    std::string s5 = "4611686018427387903";
    long long v5 = std::stoull(s5);
    TEST_RESULT("stoul", v5, 4611686018427387903ULL);

    std::string s6 = "1965.58";
    float v6 = std::stof(s6);
    TEST_FLOAT_RESULT("stof", fabsf, float, v6, 1965.58);

    std::string s7 = "1965.58";
    double v7 = std::stof(s7);
    TEST_FLOAT_RESULT("stod", fabs, double, v7, 1965.58);

    std::string s8 = "1965.58";
    long double v8 = std::stold(s8);
    TEST_FLOAT_RESULT("stold", fabsl, long double, v7, 1965.58);

    return 0;
}
