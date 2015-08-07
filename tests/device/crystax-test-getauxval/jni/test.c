#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <sys/auxv.h>

int main()
{
    unsigned long pz = getauxval(AT_PAGESZ);
    printf("AT_PAGESZ = %lu",  pz);

    return 0;
}
