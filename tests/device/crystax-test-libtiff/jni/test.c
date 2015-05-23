#include <stdio.h>
#include <tiffio.h>

int main()
{
    printf("TIFF version: %s\n", TIFFGetVersion());
    return 0;
}
