#include <common.h>
#include <string>
#include <iostream>
#include <iomanip>

GLOBAL
int test_wstring_construct()
{
    printf("1..1\n");

    std::wstring data = L"abcdefghijkl";
    printf("data.size(): %d\n", data.size());
    assert(data.size() == 12);
    assert(data[0] == L'a');
    assert(data[1] == L'b');
    assert(data[2] == L'c');
    assert(data[3] == L'd');
    assert(data[4] == L'e');
    assert(data[5] == L'f');
    assert(data[6] == L'g');
    assert(data[7] == L'h');
    assert(data[8] == L'i');
    assert(data[9] == L'j');
    assert(data[10] == L'k');
    assert(data[11] == L'l');

    printf("ok 1 - wstring_construct\n");
    return 0;
}

GLOBAL
int test_wstring_erase()
{
    printf("1..1\n");

    std::wstring data = L"abcdefghijkl";
    data.erase(data.begin(), data.begin() + 1);
    assert(data == L"bcdefghijkl");

    printf("ok 1 - wstring_erase\n");
    return 0;
}
