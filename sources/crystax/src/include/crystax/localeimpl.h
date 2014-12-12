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

#ifndef __CRYSTAX_LOCALEIMPL_H_6A027653796D422F8D10248B5E30AD39
#define __CRYSTAX_LOCALEIMPL_H_6A027653796D422F8D10248B5E30AD39

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stdint.h>

__BEGIN_DECLS

typedef struct {
    uint8_t *data;
    size_t size;
    int compressed;
} __crystax_locale_blob_t;

typedef struct {
    const char *encoding;
    struct {
        const char *alias;
        __crystax_locale_blob_t blob;
    } data[_LC_LAST];
} __crystax_locale_data_t;

int __crystax_locale_init();

size_t __crystax_locale_table_size();
__crystax_locale_data_t * __crystax_locale_data(size_t idx);

int __crystax_locale_load(const char *encoding, int type, void **buf, size_t *bufsize);
int __crystax_locale_loads(const char *encoding, const char *type, void **buf, size_t *bufsize);

__END_DECLS

#endif /* __CRYSTAX_LOCALEIMPL_H_6A027653796D422F8D10248B5E30AD39 */
