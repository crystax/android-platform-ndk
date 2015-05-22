#include <stdio.h>
#include <png.h>

int main()
{
    printf("libpng version: (%lu):%s\n", (unsigned long)png_access_version_number(), png_get_header_version(NULL));
    return 0;
}
