#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && (__GNUC_PATCHLEVEL__ == 3))
#define CRYSTAX_TOTAL_TESTS 9
#else
#define CRYSTAX_TOTAL_TESTS 7
#endif

#define CRYSTAX_STRINGIZE(x) CRYSTAX_STRINGIZE_HELPER(x)
#define CRYSTAX_STRINGIZE_HELPER(x) #x

printf("1.." CRYSTAX_STRINGIZE(CRYSTAX_TOTAL_TESTS) " - " CRYSTAX_TEST_NAME "\n");

DO_PRINTF_TEST(1,  "abcdef",              "%S",  L"abcdef");
DO_PRINTF_TEST(2,  "abcdef",              "%ls", L"abcdef");
DO_PRINTF_TEST(3,  "z",                   "%c",   'z');
DO_PRINTF_TEST(4,  "z",                   "%C",  (wint_t)L'z');
DO_PRINTF_TEST(5,  "z",                   "%lc", (wint_t)L'z');
DO_PRINTF_TEST(6, "0x1.edd2f1a9fbe77p+6", "%a",  123.456);
DO_PRINTF_TEST(7, "0X1.EDD2F1A9FBE77P+6", "%A",  123.456);
/* do not run these tests for GCC 4.4.3 */
#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && (__GNUC_PATCHLEVEL__ == 3))
DO_PRINTF_TEST(8, "0x1.edd3c07fb400bp+6", "%La", 123.4567890123L);
DO_PRINTF_TEST(9, "0X1.EDD3C07FB400BP+6", "%LA", 123.4567890123L);
#endif

#undef CRYSTAX_STRINGIZE
#undef CRYSTAX_STRINGIZE_HELPER
#undef CRYSTAX_TOTAL_TESTS
#undef CRYSTAX_TEST_NAME
#undef DO_PRINTF_TEST
