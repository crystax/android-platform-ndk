#include <byteswap.h>

#include "assert.h"

#ifndef swap16
#error swap16 not defined
#endif

#ifndef swap32
#error swap32 not defined
#endif

#ifndef swap64
#error swap64 not defined
#endif

void test_macros()
{
    assert(swap16(0) == 0);
    assert(swap16(0x0102) == 0x0201);
    assert(swap32(0) == 0);
    assert(swap32(0x01020304) == 0x04030201);
    assert(swap64(0) == 0);
    assert(swap64(0x0102030405060708ULL) == 0x0807060504030201ULL);
}
