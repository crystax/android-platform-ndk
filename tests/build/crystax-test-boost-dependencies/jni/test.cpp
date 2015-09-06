#include <iostream>
#include <iomanip>
#include <boost/regex.hpp>

int main()
{
    boost::regex re("(\\w+)-(\\d+)-(\\w+)-(\\d+)");
    boost::smatch res;

    std::cout << std::boolalpha << boost::regex_match(std::string("AAAA-12222-BBBBB-44455"),  res, re) << "\n";

    return 0;
}
