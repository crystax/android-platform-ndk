#include <stdio.h>
#include <wchar.h>
#include <assert.h>

int main()
{
    printf("1..1 - wcslen_cpp\n");

    wchar_t const *s;
    size_t len;

    s = L"abcdefghijkl";
    len = wcslen(s);
    printf("len: %lu\n", (unsigned long)len);
    assert(len == 12);

    printf("ok 1 - wcslen_cpp\n");
    return (0);
}
