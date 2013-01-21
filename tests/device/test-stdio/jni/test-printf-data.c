#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && (__GNUC_PATCHLEVEL__ == 3))
printf("1..9\n");
#else
printf("1..7\n");
#endif
DO_PRINTF_TEST(1,  "abcdef",              "%S",  L"abcdef");
DO_PRINTF_TEST(2,  "abcdef",              "%ls", L"abcdef");
DO_PRINTF_TEST(3,  "z",                   "%c",   'z');
DO_PRINTF_TEST(4,  "z",                   "%C",  L'z');
DO_PRINTF_TEST(5,  "z",                   "%lc", L'z');
DO_PRINTF_TEST(6, "0x1.edd2f1a9fbe77p+6", "%a",  123.456);
DO_PRINTF_TEST(7, "0X1.EDD2F1A9FBE77P+6", "%A",  123.456);
/* do not run these tests for GCC 4.4.3 */
#if !((__GNUC__ == 4) && (__GNUC_MINOR__ == 4) && (__GNUC_PATCHLEVEL__ == 3))
DO_PRINTF_TEST(8, "0x1.edd3c07fb400bp+6", "%La", 123.4567890123L);
DO_PRINTF_TEST(9, "0X1.EDD3C07FB400BP+6", "%LA", 123.4567890123L);
#endif
