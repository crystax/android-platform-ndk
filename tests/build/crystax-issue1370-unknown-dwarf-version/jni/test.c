#include <inttypes.h>
#include <sys/endian.h>

int16_t f16(int16_t i)
{
    return __swap16(i);
}

int main()
{
    return f16((int16_t)1);
}
