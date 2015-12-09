/*
 * Copyright (c) 2011-2015 CrystaX.
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
 * THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX.
 */

#include <string.h>
#if __APPLE__
#include <xlocale.h>
#endif
#if __gnu_linux__
#include <locale.h>
#endif

#if __ANDROID__
#if !__LIBCRYSTAX_STRING_H_XLOCALE_H_INCLUDED
#error Extended locale support not enabled in string.h
#endif
#endif

#ifndef NULL
#error 'NULL' not defined
#endif

#include "helper.h"

#define CHECK(type) type JOIN(string_check_type_, __LINE__)

CHECK(size_t);
CHECK(locale_t);

void string_check_functions(char *cp, const char *ccp,
        void *vp, const void *cvp, size_t sz, locale_t l)
{
#if __XSI_VISIBLE
    (void)memccpy(vp, cvp, 0, sz);
#endif
    (void)memchr(cvp, 0, sz);
    (void)memcmp(cvp, cvp, sz);
    (void)memcpy(vp, cvp, sz);
    (void)memmove(vp, cvp, sz);
    (void)memset(vp, 0, sz);
    (void)stpcpy(cp, ccp);
#if !__APPLE__ || MAC_OS_X_VERSION_MIN_REQUIRED >= 1090
    (void)stpncpy(cp, ccp, sz);
#endif
#if __APPLE__ || __ANDROID__
    (void)strcasestr(ccp, ccp);
    (void)strcasestr_l(ccp, ccp, l);
#endif
    (void)strcat(cp, ccp);
    (void)strchr(ccp, 0);
    (void)strcmp(ccp, ccp);
    (void)strcoll(ccp, ccp);
    (void)strcoll_l(ccp, ccp, l);
    (void)strcpy(cp, ccp);
    (void)strcspn(ccp, ccp);
    (void)strdup(ccp);
    (void)strerror(0);
#if !__APPLE__
    (void)strerror_l(0, l);
#endif
    (void)strerror_r(0, cp, sz);
    (void)strlen(ccp);
    (void)strncat(cp, ccp, sz);
    (void)strncmp(ccp, ccp, sz);
    (void)strncpy(cp, ccp, sz);
#if !__APPLE__ || defined(__MAC_10_7)
    (void)strndup(ccp, sz);
    (void)strnlen(ccp, sz);
#endif
    (void)strpbrk(ccp, ccp);
    (void)strrchr(ccp, 0);
    (void)strsignal(0);
    (void)strspn(ccp, ccp);
    (void)strstr(ccp, ccp);
    (void)strtok(cp, ccp);
    (void)strtok_r(cp, ccp, (char**)1234);
    (void)strxfrm(cp, ccp, sz);
    (void)strxfrm_l(cp, ccp, sz, l);
}
