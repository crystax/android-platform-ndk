#include "lib-static.hpp"
#include "lib-shared.hpp"

#include <iostream>

int main()
{
    libShared::getValue() = 100;
    libStatic::getValue() = 200;

    int v1 = libShared::getValue();
    int v2 = libStatic::getValue();
    std::cout << "shared: " << v1 << "\n"
        << "static: " << v2 << std::endl;

    if (v1 != v2) {
        std::cerr << "*** ERROR: " << v1 << " != " << v2 << "\n";
        return 1;
    }

    return 0;
}
