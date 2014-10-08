printf(CRYSTAX_TEST_NAME "() - begin\n");

DO_PRINTF_TEST(1,  "abcdef",              "%S",  L"abcdef");
DO_PRINTF_TEST(2,  "abcdef",              "%ls", L"abcdef");
DO_PRINTF_TEST(3,  "z",                   "%c",   'z');
DO_PRINTF_TEST(4,  "z",                   "%C",  (wint_t)L'z');
DO_PRINTF_TEST(5,  "z",                   "%lc", (wint_t)L'z');
DO_PRINTF_TEST(6, "0x1.edd2f1a9fbe77p+6", "%a",  123.456);
DO_PRINTF_TEST(7, "0X1.EDD2F1A9FBE77P+6", "%A",  123.456);

/* There is no good support of long double type in Android, so enable
 * these test cases only for non-Android OS
 */
#if !defined(__ANDROID__)
DO_PRINTF_TEST(8, "0xf.6e9e03fda00541dp+3", "%La", 123.4567890123L);
DO_PRINTF_TEST(9, "0XF.6E9E03FDA00541DP+3", "%LA", 123.4567890123L);
#endif

printf(CRYSTAX_TEST_NAME "() - ok\n");

#undef CRYSTAX_TEST_NAME
#undef DO_PRINTF_TEST
