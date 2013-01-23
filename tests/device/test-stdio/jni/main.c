#define DO_STDIO_TEST(name) if (test_ ## name () != 0) return 1

int main()
{
    /* int sz1 = sizeof(123.4567890123); */
    /* int sz2 = sizeof(123.4567890123L); */
    /* int sz3 = sizeof(long double); */

    /* printf("!!! sizeof(123.4567890123)   = %d\n",  sz1); */
    /* printf("!!! sizeof(123.4567890123L)  = %d\n",  sz2); */
    /* printf("!!! sizeof(long double)      = %d\n",  sz3); */

    DO_STDIO_TEST(printf);
    DO_STDIO_TEST(fprintf);
    DO_STDIO_TEST(sprintf);
    DO_STDIO_TEST(snprintf);

    DO_STDIO_TEST(getline);
    DO_STDIO_TEST(getdelim);

    return 0;
}
