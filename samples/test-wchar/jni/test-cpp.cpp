#include <iostream>
#include <iomanip>

extern "C"
void test_cpp()
{
    std::cout << "sizeof(wchar_t): " << sizeof(wchar_t) << std::endl;
}
