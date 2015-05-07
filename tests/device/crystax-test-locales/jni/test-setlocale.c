#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void test(int category, const char *locale)
{
    const char *cs;
    const char *nl;

    switch (category)
    {
#define CSD(x) case x : cs = #x ; break
        CSD(LC_ALL);
        CSD(LC_COLLATE);
        CSD(LC_CTYPE);
        CSD(LC_MONETARY);
        CSD(LC_NUMERIC);
        CSD(LC_TIME);
        CSD(LC_MESSAGES);
#undef CSD
    default:
        fprintf(stderr, "Unknown locale category: %d\n", category);
        abort();
    }

    fprintf(stderr, "checking %s / %s ... ", cs, locale);

    nl = setlocale(category, locale);
    if (nl == NULL || strcmp(nl, locale) != 0) {
        fprintf(stderr, "FAILED\n");
        abort();
    }

    fprintf(stderr, "OK\n");
}

int main()
{
    size_t i, j;

    int categories[] = {
        LC_ALL,
        LC_COLLATE,
        LC_CTYPE,
        LC_MONETARY,
        LC_NUMERIC,
        LC_TIME,
        LC_MESSAGES,
    };

    const char *locales[] = {
        "C",
#if !__gnu_linux__
        "POSIX",
#endif
        "en_US.UTF-8",
        "fi_FI.UTF-8",
        "sv_SE.ISO8859-1",
#if !__gnu_linux__
        "no_NO.ISO8859-15",
#endif
        "ru_RU.CP1251",
        "ru_RU.UTF-8",
        "tr_TR.UTF-8",
        "zh_CN.UTF-8",
    };

    for (i = 0; i < sizeof(categories)/sizeof(categories[0]); ++i)
        for (j = 0; j < sizeof(locales)/sizeof(locales[0]); ++j)
            test(categories[i], locales[j]);

    return 0;
}
