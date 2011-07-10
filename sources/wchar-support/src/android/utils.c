#include <sys/cdefs.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <jni.h>

#include "android.h"

int __android_mb_cur_max()
{
    /* TODO: return correct values depending on locale */
    return 1;
}

const char *_getprogname()
{
    /* TODO: implement */
    return "APP";
}

__locale_data_t *
android_get_locale_data(int type, const char *encoding)
{
    /* TODO: implement */
    return NULL;
}

__locale_data_t *
android_get_part_locale_data(const char *encoding, const char *data)
{
    /* TODO: implement */
    return NULL;
}
