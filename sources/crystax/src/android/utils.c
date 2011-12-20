#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <jni.h>

#include "android.h"

#if 0
#define LC_ALL    0
#define LC_COLLATE  1
#define LC_CTYPE  2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME   5
#define LC_MESSAGES 6
#endif

__whole_locale_data_t __whole_locale_data[] =
{
    {"UTF-8", {0}},
    {"el_GR.UTF-8", {
        {0},                               /* LC_ALL */
        {1, .aliasto = "la_LN.US-ASCII"},  /* LC_COLLATE */
        {1, .aliasto = "UTF-8"},           /* LC_CTYPE */
        {1, .aliasto = "el_GR.ISO8859-7"}, /* LC_MONETARY */
        {1, .aliasto = "el_GR.ISO8859-7"}, /* LC_NUMERIC */
        {0},                               /* LC_TIME */
        {0}                                /* LC_MESSAGES */
    }},
    {"el_GR.ISO8859-7", {0}},
    {"la_LN.US-ASCII", {
        {0},                               /* LC_ALL */
        {0},                               /* LC_COLLATE */
        {0},                               /* LC_CTYPE */
        {0},                               /* LC_MONETARY */
        {0},                               /* LC_NUMERIC */
        {1, .aliasto = "la_LN.ISO8859-1"}, /* LC_TIME */
        {0}                                /* LC_MESSAGES */
    }},
    {"la_LN.ISO8859-1", {0}},
};

__whole_locale_data_t *__lookup_whole_locale_data(const char *encoding)
{
    __whole_locale_data_t *wd;
    size_t i, lim;

    if (encoding == NULL || *encoding == '\0')
        return NULL;

    for (i = 0, lim = sizeof(__whole_locale_data)/sizeof(__whole_locale_data[0]);
        i != lim; ++i)
    {
        wd = &__whole_locale_data[i];
        if (strcmp(wd->encoding, encoding) != 0)
            continue;
        return wd;
    }
    return NULL;
}

static int __init_locale_data()
{
    static int __initialized = 0;
    if (__initialized)
        return 1;

    if (!_UTF8_locale_data_init()) return 0;
    if (!_el_GR_ISO88597_locale_data_init()) return 0;
    if (!_la_LN_USASCII_locale_data_init()) return 0;
    if (!_la_LN_ISO88591_locale_data_init()) return 0;

    __initialized = 1;
    return 1;
}

int __android_mb_cur_max()
{
    /* TODO: return correct values depending on locale */
    DBG("__android_mb_cur_max\n");
    return 1;
}

const char *_getprogname()
{
    /* TODO: implement */
    DBG("_getprogname\n");
    return "APP";
}

__locale_data_t *
android_get_locale_data(int type, const char *encoding)
{
    __whole_locale_data_t *wd;
    __locale_data_t *ld;
    char *aliasto;

    DBG("android_get_locale_data: type=%d, encoding=%s\n", type, encoding);

    if (!__init_locale_data()) return NULL;

    wd = __lookup_whole_locale_data(encoding);
    DBG("android_get_locale_data: wd=%p", wd);
    if (wd == NULL)
        return NULL;

    if (wd->data[type].alias)
    {
        DBG("android_get_locale_data: it's alias to %s", wd->data[type].aliasto);
        return android_get_locale_data(type, wd->data[type].aliasto);
    }

    ld = &(wd->data[type].data);
    DBG("android_get_locale_data: ld=%p", ld);
    return ld;
}

__locale_data_t *
android_get_part_locale_data(const char *encoding, const char *data)
{
    /* TODO: implement */
    DBG("android_get_part_locale_data: encoding=%s, data=%s\n", encoding, data);
    if (!__init_locale_data()) return NULL;
    return NULL;
}

mbstate_t *mbstate_for(FILE *fp)
{
    /* TODO: implement */
    static mbstate_t mbs;

    DBG("mbstate_for: fp=%x\n", fp);
    return &mbs;
}

#ifdef __i386__
fp_prec_t fpsetprec(fp_prec_t p)
{
    /* TODO: implement */
    return p;
}
#endif
