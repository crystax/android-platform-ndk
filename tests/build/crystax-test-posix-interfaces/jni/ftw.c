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

#if __ANDROID__
#include <android/api-level.h>
#endif

#if !__ANDROID__ || __ANDROID_API__ >= 21

#include <ftw.h>

#include "gen/ftw.inc"

#include "helper.h"

#define CHECK(type) type JOIN(ftw_check_type_, __LINE__)

CHECK(struct FTW);

void ftw_check_FTW_fields(struct FTW *s)
{
    s->base  = 0;
    s->level = 0;
}

void ftw_check_functions()
{
    typedef int (*ftw_cb_t)(const char *, const struct stat *, int);
    typedef int (*nftw_cb_t)(const char *, const struct stat *, int, struct FTW *);

    (void)ftw((const char*)1234, (ftw_cb_t)1234, 0);
    (void)nftw((const char*)1234, (nftw_cb_t)1234, 0, 0);
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 21 */
