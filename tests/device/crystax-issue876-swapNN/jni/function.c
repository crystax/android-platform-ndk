#include <byteswap.h>

#include "assert.h"

/* Undef swapNN macros to check if they could be used as externally linked functions */
#undef swap16
#undef swap32
#undef swap64

extern uint16_t swap16(uint16_t x);
extern uint32_t swap32(uint32_t x);
extern uint64_t swap64(uint64_t x);

void test_functions()
{
    assert(swap16(0) == 0);
    assert(swap16(0x0102) == 0x0201);
    assert(swap32(0) == 0);
    assert(swap32(0x01020304) == 0x04030201);
    assert(swap64(0) == 0);
    assert(swap64(0x0102030405060708ULL) == 0x0807060504030201ULL);
}
