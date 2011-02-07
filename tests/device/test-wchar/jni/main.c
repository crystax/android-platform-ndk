#ifdef DO_WCHAR_TEST
#undef DO_WCHAR_TEST
#endif
#define DO_WCHAR_TEST(name) if (test_ ## name () != 0) return 1

int main()
{
    DO_WCHAR_TEST(btowc);
    DO_WCHAR_TEST(iswctype);
    DO_WCHAR_TEST(mblen);
    DO_WCHAR_TEST(mbrlen);
    DO_WCHAR_TEST(mbrtowc);
    DO_WCHAR_TEST(mbsnrtowcs);
    DO_WCHAR_TEST(mbsrtowcs);
    DO_WCHAR_TEST(mbstowcs);
    DO_WCHAR_TEST(mbtowc);
    DO_WCHAR_TEST(towctrans);
    DO_WCHAR_TEST(wcrtomb);
    DO_WCHAR_TEST(wcscasecmp);
    DO_WCHAR_TEST(wcsnlen);
    DO_WCHAR_TEST(wcsnrtombs);
    DO_WCHAR_TEST(wcsrtombs);
    DO_WCHAR_TEST(wcstombs);
    DO_WCHAR_TEST(wctomb);
    return 0;
}
