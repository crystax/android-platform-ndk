#include <iostream>
#include <crystax.h>

int main()
{
    if (crystax_jvm() != NULL) {
        std::cerr << "ERROR: crystax_jvm() returned not NULL pointer!" << std::endl;
        return 1;
    }

    return 0;
}
