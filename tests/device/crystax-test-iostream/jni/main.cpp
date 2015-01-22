// When built with GCC 4.7 for armeabi and run on armeabi emulator under
// Windows or Mac OS X this test hangs.
// The reason -- 'include <iostream>'.
// That is why this test is.

#include <iostream>
#include <iomanip>
#include <fstream>

int main()
{
    std::cout << "iostream - begin" << std::endl;
    std::cout << "OK" << std::endl;

    std::ofstream fileOut("out");
    fileOut << "34 34 34 34 34 34 34 34 34 34 34 34 34 34 34 34 34 34";
    fileOut.close();

    std::ifstream fileIn("out");
    fileIn.seekg(12,std::ios::beg);
    uint16_t data;
    fileIn >> data;

    if (data != 34) {
        std::cerr << "ERROR: read '" << data << "' instead of 34" << std::endl;
        return 1;
    }

    return 0;
}
