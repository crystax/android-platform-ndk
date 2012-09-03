#include <common.h>
#include <string>
#include <iostream>
#include <iomanip>

static int test_wstring_base()
{
    printf("1..1\n");

    assert(sizeof(std::wstring::value_type) == 4);
    std::wstring wstr(L"sdcard");
    assert(wstr.size() == 6);
    assert(wstr[0] == L's');
    assert(wstr[1] == L'd');
    assert(wstr[2] == L'c');
    assert(wstr[3] == L'a');
    assert(wstr[4] == L'r');
    assert(wstr[5] == L'd');

    printf("ok 1 - wstring_size\n");
    return 0;
}

static int test_wstring_construct()
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

static int test_wstring_erase()
{
    printf("1..5\n");

    setlocale(LC_CTYPE, "UTF-8");

    std::wstring data = L"abcdefghijkl";
    assert(data == L"abcdefghijkl");
    data.erase(1);
    assert(data.size() == 1);
    assert(data == L"a");
    printf("ok 1 - wstring_erase\n");

    data = L"abcdefghijkl";
    assert(data == L"abcdefghijkl");
    data.erase(2);
    assert(data.size() == 2);
    assert(data == L"ab");
    printf("ok 2 - wstring_erase\n");

    data.erase(0, 1);
    assert(data.size() == 1);
    assert(data == L"b");
    printf("ok 3 - wstring_erase\n");

    data = L"abcdefghijkl";
    assert(data.size() == 12);
    assert(data == L"abcdefghijkl");
    printf("data.c_str(): %p\n", data.c_str());
    for (wchar_t const *s = data.c_str(); *s != 0; ++s)
        printf("0x%x ", (int)*s);
    printf("\n");
    data.erase(0, 1);
    for (wchar_t const *s = data.c_str(); *s != 0; ++s)
        printf("0x%x ", (int)*s);
    printf("\n");
    printf("data.c_str(): %p\n", data.c_str());
    assert(data.size() == 11);
    assert(data == L"bcdefghijkl");
    printf("ok 4 - wstring_erase\n");

    data = L"abcdefghijkl";
    assert(data.size() == 12);
    assert(data == L"abcdefghijkl");
    data.erase(data.begin(), data.begin() + 1);
    assert(data.size() == 11);
    assert(data[0] == L'b');
    assert(data[1] == L'c');
    assert(data[2] == L'd');
    printf("data[3] = 0x%x\n", (int)data[3]);
    assert(data[3] == L'e');
    assert(data[4] == L'f');
    assert(data[5] == L'g');
    assert(data[6] == L'h');
    assert(data[7] == L'i');
    assert(data[8] == L'j');
    assert(data[9] == L'k');
    assert(data[10] == L'l');
    assert(data[11] == L'\0');
    assert(data == L"bcdefghijkl");
    printf("ok 5 - wstring_erase\n");

    return 0;
}

GLOBAL
int test_wstring_all()
{
#define DO_WSTRING_TEST(x) if (test_wstring_ ## x ()) return 1
    DO_WSTRING_TEST(base);
    DO_WSTRING_TEST(construct);
    DO_WSTRING_TEST(erase);
#undef DO_WSTRING_TEST
    return 0;
}
