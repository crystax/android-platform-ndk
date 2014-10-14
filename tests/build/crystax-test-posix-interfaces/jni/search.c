/*
 * Copyright (c) 2011-2014 CrystaX .NET.
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
 * THIS SOFTWARE IS PROVIDED BY CrystaX .NET ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX .NET OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of CrystaX .NET.
 */

#if __ANDROID__
#include <android/api-level.h>
#endif

#if !__ANDROID__ || __ANDROID_API__ >= 21

#include <search.h>

#include "helper.h"

#define CHECK(type) type JOIN(search_check_type_, __LINE__)

#if __ANDROID__

typedef struct entry
{
    char *key;
    void *data;
} ENTRY;

typedef enum {FIND, ENTER} ACTION;

int hcreate(size_t n)
{
    (void)n;
    return -1;
}

void hdestroy()
{}

ENTRY *hsearch(ENTRY e, ACTION a)
{
    (void)e;
    (void)a;
    return (ENTRY*)0;
}

#endif /* __ANDROID__ */

CHECK(struct entry);
CHECK(ENTRY);
CHECK(ACTION);
CHECK(VISIT);
CHECK(size_t);

void search_check_ENTRY_fields(ENTRY *s)
{
    s->key  = (char*)0;
    s->data = (void*)0;
}

void search_check_ACTION_values()
{
#undef CHECK
#define CHECK(v) ACTION JOIN(search_check_ACTION_value_, __LINE__) = v; (void)JOIN(search_check_ACTION_value_, __LINE__)
    CHECK(FIND);
    CHECK(ENTER);
}

void search_check_VISIT_values()
{
#undef CHECK
#define CHECK(v) VISIT JOIN(search_check_VISIT_value_, __LINE__) = v; (void)JOIN(search_check_VISIT_value_, __LINE__)
    CHECK(preorder);
    CHECK(postorder);
    CHECK(endorder);
    CHECK(leaf);
}

void search_check_functions(ENTRY e, ACTION a)
{
    typedef int (*cb_t)(const void *, const void *);
    typedef void (*twalk_cb_t)(const void *, VISIT, int );

    (void)hcreate((size_t)0);
    (void)hdestroy();
    (void)hsearch(e, a);
    (void)insque((void*)0, (void*)0);
    (void)lfind((const void*)0, (const void*)0, (size_t*)0, (size_t)0, (cb_t)0);
    (void)lsearch((const void*)0, (void*)0, (size_t*)0, (size_t)0, (cb_t)0);
    (void)remque((void*)0);
    (void)tdelete((const void*)0, (void**)0, (cb_t)0);
    (void)tfind((const void*)0, (void * const *)0, (cb_t)0);
    (void)tsearch((const void*)0, (void**)0, (cb_t)0);
    (void)twalk((const void*)0, (twalk_cb_t)0);
}

#endif /* !__ANDROID__ || __ANDROID_API__ >= 21 */
