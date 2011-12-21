#include <iostream>
#include <sstream>

int main()
{
    std::stringstream ss;
    ss << 3.14;
    std::cout << ss.str() << "\n";
    std::cout << 3.15 << "\n";
    return 0;
}
