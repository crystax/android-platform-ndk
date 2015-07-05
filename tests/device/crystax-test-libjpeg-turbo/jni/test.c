#include <stdio.h>
#include <jpeglib.h>

int main()
{
    struct jpeg_compress_struct cinfo;
    jpeg_create_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return 0;
}
