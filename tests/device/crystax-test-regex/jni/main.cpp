#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)
#define HAVE_STD_REGEX 1
#else
#define HAVE_STD_REGEX 0
#endif
#endif

#if HAVE_STD_REGEX
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
#else /* !HAVE_STD_REGEX */
int main() { return 0; }
#endif /* !HAVE_STD_REGEX */
