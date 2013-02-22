#ifndef MYEXCEPTION_H__
#define MYEXCEPTION_H__

#include <stdio.h>
#include <string>
#include <stdexcept>
/* #include <iostream> */

class my_exception : public std::runtime_error
{
public:
    my_exception(std::string const& w)
        : std::runtime_error(w)
    {
        printf("yep! i am in constructor\n");
        /* std::cout << "yep! i am in constructor" << std::endl;  */
    }

    virtual ~my_exception() throw()
    {
        printf("yep! i am in desctuctor\n");
        /* std::cout << "yep! i am in desctuctor" << std::endl;  */
    }
};

#endif //MYEXCEPTION_H__
