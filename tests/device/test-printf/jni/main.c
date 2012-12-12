#define DO_PRINTF_TEST(name) if (test_ ## name () != 0) return 1

int main()
{
    DO_PRINTF_TEST(printf);
    DO_PRINTF_TEST(fprintf);
    DO_PRINTF_TEST(sprintf);
    DO_PRINTF_TEST(snprintf);

    return 0;
}
