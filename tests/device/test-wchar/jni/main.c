extern int test_wcslen_c(void);
extern int test_wcslen_cpp(void);
extern int test_btowc(void);
extern int test_iswctype(void);
extern int test_mblen(void);
extern int test_mbrlen(void);
extern int test_mbrtowc(void);
extern int test_mbsnrtowcs(void);
extern int test_mbsrtowcs(void);
extern int test_mbstowcs(void);
extern int test_mbtowc(void);
extern int test_ms_wprintf(void);
extern int test_towctrans(void);
extern int test_wcrtomb(void);
extern int test_wcscasecmp(void);
extern int test_wcsnlen(void);
extern int test_wcsnrtombs(void);
extern int test_wcsrtombs(void);
extern int test_wcstombs(void);
extern int test_wctomb(void);
extern int test_wstring_all(void);
extern int test_wprintf_all(void);
extern int test_wscanf_all(void);


#ifdef DO_WCHAR_TEST
#undef DO_WCHAR_TEST
#endif
#define DO_WCHAR_TEST(name) if (test_ ## name () != 0) return 1

int main()
{
    DO_WCHAR_TEST(wcslen_c);
    DO_WCHAR_TEST(wcslen_cpp);

    DO_WCHAR_TEST(btowc);
    DO_WCHAR_TEST(iswctype);
    DO_WCHAR_TEST(mblen);
    DO_WCHAR_TEST(mbrlen);
    DO_WCHAR_TEST(mbrtowc);
    DO_WCHAR_TEST(mbsnrtowcs);
    DO_WCHAR_TEST(mbsrtowcs);
    DO_WCHAR_TEST(mbstowcs);
    DO_WCHAR_TEST(mbtowc);
    DO_WCHAR_TEST(ms_wprintf);
    DO_WCHAR_TEST(towctrans);
    DO_WCHAR_TEST(wcrtomb);
    DO_WCHAR_TEST(wcscasecmp);
    DO_WCHAR_TEST(wcsnlen);
    DO_WCHAR_TEST(wcsnrtombs);
    DO_WCHAR_TEST(wcsrtombs);
    DO_WCHAR_TEST(wcstombs);
    DO_WCHAR_TEST(wctomb);
    DO_WCHAR_TEST(wstring_all);
    DO_WCHAR_TEST(wprintf_all);
#if 0
    /* Temporarily disabled */
    DO_WCHAR_TEST(wscanf_all);
#endif
    return 0;
}
