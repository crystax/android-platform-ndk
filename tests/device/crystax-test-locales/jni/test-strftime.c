#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test(const char *locale, struct tm *tm, const char *fmt, const char *expected)
{
    char buf[1024];

    setlocale(LC_TIME, locale);
    strftime(buf, sizeof(buf), fmt, tm);
    if (strcmp(buf, expected) != 0)
    {
        fprintf(stderr, "ERROR: strftime(\"%s\") returned [%s] --> expected [%s] for locale \"%s\"\n",
                fmt, buf, expected, locale);
        abort();
    }
}

int main()
{
    const char *fmt = "%x";
    time_t t = (time_t)1411902244;
    struct tm *tm = localtime(&t);

    test("C",               tm, fmt, "09/28/14");
    test("en_US.UTF-8",     tm, fmt, "09/28/2014");
    test("ru_RU.CP1251",    tm, fmt, "28.09.2014");
    test("sv_SE.ISO8859-1", tm, fmt, "2014-09-28");
#if !__gnu_linux__
    test("tr_TR.ISO8859-9", tm, fmt, "28/09/2014");
    test("zh_CN.UTF-8",     tm, fmt, "2014/09/28");
#endif

    return 0;
}
