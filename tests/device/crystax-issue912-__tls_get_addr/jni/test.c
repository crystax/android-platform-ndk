#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>

void tga_set_value(int v);
int tga_value();

int main()
{
    tga_set_value(1);
    assert(tga_value() == 1);

    return 0;
}
