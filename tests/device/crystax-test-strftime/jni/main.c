#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(const char *locale, struct tm *tm, const char *fmt, const char *expected)
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
    const char *locale;
    const char *fmt = "%x";
    time_t t = (time_t)1411859044;
    struct tm *tm = localtime(&t);

    print("C", tm, fmt, "09/28/14");
    print("en_US.UTF-8", tm, fmt, "09/28/2014");
    print("ru_RU.CP1251", tm, fmt, "28.09.2014");
    print("sv_SE.ISO8859-1", tm, fmt, "2014-09-28");
    print("tr_TR.ISO8859-9", tm, fmt, "28/09/2014");

    return 0;
}
