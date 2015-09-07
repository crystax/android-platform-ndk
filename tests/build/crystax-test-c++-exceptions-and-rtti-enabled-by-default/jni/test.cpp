#include <typeinfo>
#include <stdexcept>
#include <stdio.h>

class Foo { int x; };

static void bar()
{
    throw std::runtime_error("");
}

int main(void)
{
    printf("%s\n", typeid(Foo).name()); // will fail without -frtti
    try
    {
        bar();
    }
    catch (std::exception &)
    {
        // Nothing
    }

    return 0;
}
