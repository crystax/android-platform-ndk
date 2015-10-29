#include <string>
#include <regex>
#include <iostream>

struct Confs{
    std::string regexp;
};

class foo
{
    public:
        static int search(const std::string& phrase);
        static const Confs configs[];
};



const Confs foo::configs[] = {
    //Regular Expresion
    {"^adb"}, //Example
    {"^qwr"}, //Example
    {"^a5eo"}, //Real
};

int foo::search(const std::string& phrase){
    for(size_t i=0; i<sizeof(configs)/sizeof(Confs); i++){
        std::regex re(configs[i].regexp);
        if (std::regex_search(phrase, re)) {
            return i;
        }
    }
    return -1;
}

int main(){
    if (foo::search("a5eo") < 0) {
        std::cerr << "FAILED" << std::endl;
        return 1;
    }

    std::cout << "OK" << std::endl;
    return 0;
}
