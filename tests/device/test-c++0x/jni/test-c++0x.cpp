#include <cerrno>
#include <cstddef>
#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <regex>

// Const expressions
constexpr int get_five() {return 5;}

// rvalue references
struct A {};
void foobar(A &&) {std::cout << __PRETTY_FUNCTION__ << std::endl;}
void foobar(A const &) {std::cout << __PRETTY_FUNCTION__ << std::endl;}

// Enum forward declaration
enum Color : char;
void foo(Color );
enum Color : char {reg, green, blue};

// Stronly typed enumerations
enum class enum1_t
{
    v1,
    v2,
    v3 = 100
};

enum class enum2_t : unsigned long
{
    // ERROR: enumerator value -0x00000000000000001 is too large for underlying type 'long unsigned int'
    //v1 = -1,
    v1 = 1,
    v2,
    v3 = 100
};

// Right angle brackets problem
template <bool Test> class TestType {};
std::vector<TestType<(1>2)>> x;

// Variadic templates
void fooprintf(const char* s)	
{
    while (s && *s) {
        if (*s=='%' && *++s!='%')   // make sure that there wasn't meant to be more arguments
                                    // %% represents plain % in a format string
             throw std::runtime_error("invalid format: missing arguments");
        std::cout << *s++;
    }
}

template<typename T, typename... Args>
void fooprintf(const char* s, T value, Args... args)
{
    while (s && *s) {
        if (*s=='%' && *++s!='%') // a format specifier (ignore which one it is)
        {
            std::cout << value; // use first non-format argument
            return fooprintf(++s, args...); // "peel off" first argument
        }
        std::cout << *s++;
    }
    throw std::runtime_error("extra arguments provided to fooprintf");
}

// New function declaration syntax
auto main() -> int
{
    // nullptr
    char *np = nullptr;
    // ERROR: cannot convert 'std::nullptr_t' to 'int' in initialization
    //int nv = nullptr;

    // New unique_ptr
    std::unique_ptr<int> p;

    // decltype
    int some_int;
    decltype(some_int) other_integer_variable = 5;

    // New for syntax
    int my_array[5] = {1, 2, 3, 4, 5};
    for (int &x : my_array) {
        x *= 2;
    }

    // Lambda functions
    auto f = [](int x, int y) { return x + y; };
    std::cout << "lambda call: " << f(1, 2) << std::endl;

    // Usage of fooprintf
    fooprintf("test1\n");
    try
    {
        fooprintf("test2: %s\n");
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    try
    {
        fooprintf("test3: %d\n", 1, 2, 3);
    }
    catch (std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    // Unicode string literals
    auto s1 = u8"test";
    auto s2 = u"test";
    auto s3 = U"test";

    // Raw string literals
    auto r1 = R"(foo " \ bar)";
    auto r2 = R"q(this is raw string literal)q";
    std::cout << "r1: " << r1 << std::endl;
    std::cout << "r2: " << r2 << std::endl;

    // Test rvalue references
    A const a;
    foobar(a);
    foobar(A());

    return 0;
}
