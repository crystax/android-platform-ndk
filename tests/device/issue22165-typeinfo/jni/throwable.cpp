#include <stdio.h>
// #include <iostream>

#include "myexception.h"
#include "throwable.h"

int throw_an_exception()
{
    printf("throw_an_exception()\n");
    // std::cout << "throw_an_exception()" << std::endl;
    throw my_exception("my exception");
}
