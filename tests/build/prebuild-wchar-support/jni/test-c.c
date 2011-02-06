#include <wctype.h>
#include <wchar.h>

#include <stdio.h>

void test_c()
{
    printf("sizeof(wchar_t): %lu\n", (unsigned long)sizeof(wchar_t));
    printf("sizeof(L'a'): %lu\n", (unsigned long)sizeof(L'a'));
}
