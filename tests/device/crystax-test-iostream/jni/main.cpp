// When built with GCC 4.7 for armeabi and run on armeabi emulator under
// Windows or Mac OS X this test hangs.
// The reason -- 'include <iostream>'.
// That is why this test is.

#include <iostream>
#include <iomanip>

int main()
{
    std::cout << "iostream - begin" << std::endl;
    std::cout << "OK" << std::endl;
    return 0;
}
