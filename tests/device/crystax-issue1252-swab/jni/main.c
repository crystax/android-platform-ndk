#include <assert.h>
#include <unistd.h>
#include <stdint.h>

int main()
{
    uint8_t src[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t dst[sizeof(src)/sizeof(src[0])];

    swab(NULL, NULL, -1);

    memset(dst, 0, sizeof(dst));
    swab(src, dst, -1);
    assert(dst[0] == 0);

    memset(dst, 0, sizeof(dst));
    swab(src, dst, 0);
    assert(dst[0] == 0);

    memset(dst, 0, sizeof(dst));
    swab(src, dst, 2);
    assert(dst[0] == 0x02);
    assert(dst[1] == 0x01);
    assert(dst[2] == 0);

    memset(dst, 0, sizeof(dst));
    swab(src, dst, 5);
    assert(dst[0] == 0x02);
    assert(dst[1] == 0x01);
    assert(dst[2] == 0x04);
    assert(dst[3] == 0x03);
    assert(dst[4] == 0);
    assert(dst[5] == 0);

    return 0;
}
