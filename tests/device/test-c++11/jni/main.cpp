#include "common.h"

int main()
{
    DO_TEST(chrono_duration_ctor);
    DO_TEST(chrono_duration_operators);
    DO_TEST(chrono_duration_count);
    DO_TEST(chrono_duration_min_max);
    DO_TEST(thread);

    return 0;
}
