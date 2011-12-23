/*
 * Copyright (c) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Dmitry Moskalchuk.
 */

#include "crystax/private.h"

#if 0
#define LC_ALL    0
#define LC_COLLATE  1
#define LC_CTYPE  2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME   5
#define LC_MESSAGES 6
#endif

int volatile __crystax_locale_init_flag = 0;
pthread_mutex_t __crystax_locale_init_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

#define __CRYSTAX_LOCALE_PART_NULL_INITIALIZER {.alias = 0, .aliasto = 0, .data = {.data = 0, .size = 0}}
#define __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER(a) {.alias = 1, .aliasto = a, .data = {.data = 0, .size = 0}}

__crystax_locale_whole_data_t __crystax_locale_whole_data[] =
{
    {.encoding = "UTF-8", {__CRYSTAX_LOCALE_PART_NULL_INITIALIZER}},
    {.encoding = "el_GR.UTF-8", {
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_ALL */
        __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER("la_LN.US-ASCII"),  /* LC_COLLATE */
        __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER("UTF-8"),           /* LC_CTYPE */
        __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER("el_GR.ISO8859-7"), /* LC_MONETARY */
        __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER("el_GR.ISO8859-7"), /* LC_NUMERIC */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_TIME */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER                      /* LC_MESSAGES */
    }},
    {.encoding = "el_GR.ISO8859-7", {__CRYSTAX_LOCALE_PART_NULL_INITIALIZER}},
    {.encoding = "la_LN.US-ASCII", {
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_ALL */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_COLLATE */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_CTYPE */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_MONETARY */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER,                     /* LC_NUMERIC */
        __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER("la_LN.ISO8859-1"), /* LC_TIME */
        __CRYSTAX_LOCALE_PART_NULL_INITIALIZER                      /* LC_MESSAGES */
    }},
    {.encoding = "la_LN.ISO8859-1", {__CRYSTAX_LOCALE_PART_NULL_INITIALIZER}},
};

#undef __CRYSTAX_LOCALE_PART_NULL_INITIALIZER
#undef __CRYSTAX_LOCALE_PART_ALIAS_INITIALIZER

extern int __crystax_locale_UTF8_init();
extern int __crystax_locale_el_GR_ISO88597_init();
extern int __crystax_locale_la_LN_USASCII_init();
extern int __crystax_locale_la_LN_ISO88591_init();

int __crystax_locale_init_impl()
{
    DBG("initialization started");

    if (__crystax_locale_UTF8_init() < 0) return -1;
    if (__crystax_locale_el_GR_ISO88597_init() < 0) return -1;
    if (__crystax_locale_la_LN_USASCII_init() < 0) return -1;
    if (__crystax_locale_la_LN_ISO88591_init() < 0) return -1;

    DBG("initialization finished");
    return 0;
}

int __crystax_locale_init()
{
    if (!__crystax_locale_init_flag)
    {
        if (pthread_mutex_lock(&__crystax_locale_init_mutex) != 0)
            return -1;
        if (!__crystax_locale_init_flag)
        {
            if (__crystax_locale_init_impl() < 0)
                return -1;
            __crystax_locale_init_flag = 1;
        }
        if (pthread_mutex_unlock(&__crystax_locale_init_mutex) != 0)
            return -1;
    }
    return 0;
}

__crystax_locale_whole_data_t *
__crystax_locale_lookup_whole_data(const char *encoding)
{
    __crystax_locale_whole_data_t *wd;
    size_t i, lim;

    if (encoding == NULL || *encoding == '\0')
        return NULL;

    for (i = 0, lim = sizeof(__crystax_locale_whole_data)/sizeof(__crystax_locale_whole_data[0]);
        i != lim; ++i)
    {
        wd = &__crystax_locale_whole_data[i];
        if (strcmp(wd->encoding, encoding) != 0)
            continue;
        return wd;
    }
    return NULL;
}

__crystax_locale_data_t *
__crystax_locale_get_data(int type, const char *encoding)
{
    __crystax_locale_whole_data_t *wd;
    __crystax_locale_data_t *ld;

    DBG("type=%d, encoding=%s\n", type, encoding);

    if (__crystax_locale_init() < 0)
        return NULL;

    wd = __crystax_locale_lookup_whole_data(encoding);
    DBG("wd=%p", wd);
    if (wd == NULL)
        return NULL;

    if (wd->data[type].alias)
    {
        DBG("it's alias to %s", wd->data[type].aliasto);
        return __crystax_locale_get_data(type, wd->data[type].aliasto);
    }

    ld = &(wd->data[type].data);
    DBG("ld=%p", ld);
    return ld;
}

__crystax_locale_data_t *
__crystax_locale_get_part_data(const char *encoding, const char *data)
{
    /* TODO: implement */
    DBG("encoding=%s, data=%s\n", encoding, data);
    if (__crystax_locale_init() < 0)
        return NULL;
    return NULL;
}
