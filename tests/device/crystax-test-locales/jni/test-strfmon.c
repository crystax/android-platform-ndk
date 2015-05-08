#include <locale.h>
#include <monetary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

void test(const char *locale, double v, const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    int matched = 0;

    fprintf(stderr, "checking %s ...\n", locale);

    assert(setlocale(LC_MONETARY, locale));
    assert(strfmon(buf, sizeof(buf), fmt, v) != -1);

    va_start(args, fmt);

    for (;;) {
        const char *expected = va_arg(args, const char *);
        if (!expected)
            break;

        const char *s;
        for (s = buf; isspace(*s); ++s);
        if (strlen(s) == strlen(expected) && memcmp(s, expected, strlen(s)) == 0) {
            fprintf(stderr, "strfmon(\"%s\") matched to \"%s\"\n", fmt, expected);
            matched = 1;
            break;
        }

        fprintf(stderr, "WARNING: strfmon(\"%s\") returned [%s] --> expected [%s] for locale \"%s\"\n",
                fmt, s, expected, locale);
    }

    if (!matched) {
        fprintf(stderr, "ERROR: None of variants above matched result of strfmon() call\n");
        abort();
    }

    va_end(args);
}

int main()
{
    double v = 13653.6783;
    const char *fmt = "%.2n";

#define C_EXPECTED      "13653.68"
#define EN_US_EXPECTED "$13,653.68"
#define SV_SE_EXPECTED "13 653,68 kr"
#define ZH_CN_EXPECTED "￥13,653.68"

#if __ANDROID__
#define FI_FI_EXPECTED "13.653,68€"
#define RU_RU_EXPECTED "13 653,68 руб."
#define TR_TR_EXPECTED "L 13.653,68"
#else
#define FI_FI_EXPECTED "13.653,68€", "13.653,68Eu", "13%653,68 €"
#define RU_RU_EXPECTED "13 653,68 руб.", "13%653.68 руб"
#define TR_TR_EXPECTED "L 13.653,68", "13.653,68 YTL"
#endif

#if !__gnu_linux__
#define NO_NO_EXPECTED "kr13.653,68"
#endif

    test("C",                v, fmt, C_EXPECTED,     NULL);
    test("en_US.UTF-8",      v, fmt, EN_US_EXPECTED, NULL);
    test("fi_FI.UTF-8",      v, fmt, FI_FI_EXPECTED, NULL);
    test("sv_SE.ISO8859-1",  v, fmt, SV_SE_EXPECTED, NULL);
    test("ru_RU.UTF-8",      v, fmt, RU_RU_EXPECTED, NULL);
    test("tr_TR.UTF-8",      v, fmt, TR_TR_EXPECTED, NULL);
    test("zh_CN.UTF-8",      v, fmt, ZH_CN_EXPECTED, NULL);

#ifdef NO_NO_EXPECTED
    test("no_NO.ISO8859-15", v, fmt, NO_NO_EXPECTED, NULL);
#endif

    return 0;
}
