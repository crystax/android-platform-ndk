#include <boost/locale.hpp>
#include <string>
#include <iostream>

extern "C" int issetugid() { return 0; }

int main()
{
    using boost::locale::conv::to_utf;
    using namespace std;
    auto data = vector<uint8_t>{ 0xCA, 0xC1, 0xCB, 0xCF, 0xC3, 0xC5, 0xD1, 0xCF, 0xD0, 0xCF, 0xD5, 0xCB, 0xCF, 0xD3, 0x00 };
    auto out_t = u8"ΚΑΛΟΓΕΡΟΠΟΥΛΟΣ";
    string text;
    copy(begin(data), end(data), back_inserter(text));
    auto encoding = string("ISO-8859-7");
    auto ret = to_utf<char>(text, encoding);
    assert(out_t == ret);
    return 0;
}
