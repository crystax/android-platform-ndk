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

/* WARNING!!! Android doesn't include ndbm.h header */

#elif __gnu_linux__

/* GNU linux doesn't include it too */

#else

#include <ndbm.h>

#ifndef DBM_INSERT
#error 'DBM_INSERT' not defined
#endif

#ifndef DBM_REPLACE
#error 'DBM_REPLACE' not defined
#endif

#include "helper.h"

#define CHECK(type) type JOIN(ndbm_check_type_, __LINE__)

CHECK(datum);
CHECK(size_t);
CHECK(DBM);

void ndbm_check_datum_fields(datum *s)
{
    s->dptr  = (void*)0;
    s->dsize = (size_t)0;
}

void ndbm_check_functions(DBM *d, datum dt)
{
    (void)dbm_clearerr(d);
    (void)dbm_close(d);
    (void)dbm_delete(d, dt);
    (void)dbm_error(d);
    (void)dbm_fetch(d, dt);
    (void)dbm_firstkey(d);
    (void)dbm_nextkey(d);
    (void)dbm_open((const char *)0, 0, (mode_t)0);
    (void)dbm_store(d, dt, dt, 0);
}

#endif
