#include <cmath>

int main()
{
    volatile double d1, d2;

    std::fpclassify(d1);
    std::isfinite(d1);
    std::isinf(d1);
    std::isnan(d1);
    std::isnormal(d1);
    std::signbit(d1);
    std::isgreater(d1, d2);
    std::isgreaterequal(d1, d2);
    std::isless(d1, d2);
    std::islessequal(d1, d2);
    std::islessgreater(d1, d2);
    std::isunordered(d1, d2);

    return 0;
}
