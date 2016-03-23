#include <stdint.h> // for PTRDIFF_MAX
#include <stddef.h> // for ptrdiff_t

int main()
{
    ptrdiff_t x = PTRDIFF_MAX;
    (void)x;
    return 0;
}
