#include <stdio.h>
// #include <iostream>
#include <stdexcept>

#include "myexception.h"
#include "throwable.h"

int main(int /*argc*/, char** /*argv*/)
{
    int result = 0;
    printf("call throw_an_exception()\n");
    //std::cout << "call throw_an_exception()" << std::endl;

    try {
        throw_an_exception();
    } catch (my_exception const& e) {
        printf("my_exception caught!\n");
        // std::cout << "my_exception caught!" << std::endl;
    } catch (std::exception const& e) {
        printf("ERROR: exception caught!\n");
        // std::cout << "ERROR: exception caught!" << std::endl;
        result = 1;
    }

    printf("finished\n");
    // std::cout << "finished" << std::endl;

    return result;
}
