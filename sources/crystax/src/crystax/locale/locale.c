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

#include <crystax/localeimpl.h>
#include <errno.h>
#include <pthread.h>
#include <bzlib.h>

static pthread_mutex_t dmtx = PTHREAD_MUTEX_INITIALIZER;

static int decompress(__crystax_locale_blob_t *blob)
{
    int rc;
    char *dst = NULL;
    unsigned dstlen = 0;

    if (blob->compressed == 0)
        return 0;

    if (pthread_mutex_lock(&dmtx) != 0)
        abort();

    if (blob->compressed)
    {
        for (dstlen = blob->size * 2 + 1024;; dstlen += blob->size + 1024)
        {
            dst = (char*)reallocf(dst, dstlen);
            if (dst == NULL)
            {
                errno = ENOMEM;
                goto err;
            }

            rc = BZ2_bzBuffToBuffDecompress(dst, &dstlen, (char*)blob->data, (unsigned)blob->size, 0, 0);
            if (rc == BZ_OUTBUFF_FULL)
                continue;
            if (rc == BZ_OK)
                break;
            errno = EFAULT;
            goto err;
        }
        blob->data = (uint8_t*)dst;
        blob->size = (size_t)dstlen;
        blob->compressed = 0;
    }

    rc = 0;
    goto success;

err:
    rc = -1;

success:
    if (pthread_mutex_unlock(&dmtx) != 0)
        abort();

    return rc;
}

static __crystax_locale_data_t *lookup(const char *encoding)
{
    __crystax_locale_data_t *d;
    size_t i, lim;

    if (encoding == NULL || *encoding == '\0')
        return NULL;

    for (i = 0, lim = __crystax_locale_table_size(); i < lim; ++i)
    {
        d = __crystax_locale_data(i);
        if (d && d->encoding && strcmp(d->encoding, encoding) == 0)
            return d;
    }
    return NULL;
}

int __crystax_locale_load(const char *encoding, int type, void **buf, size_t *bufsize)
{
    __crystax_locale_data_t *ld;

    if (__crystax_locale_init() != 0)
        return -1;

    if (type < 0 || type >= _LC_LAST)
    {
        errno = EINVAL;
        return -1;
    }

    if ((ld = lookup(encoding)) == NULL)
    {
        errno = ENOENT;
        return -1;
    }

    if (ld->data[type].alias)
        return __crystax_locale_load(ld->data[type].alias, type, buf, bufsize);

    if (decompress(&(ld->data[type].blob)) < 0)
        return -1;

    if (buf) *buf = ld->data[type].blob.data;
    if (bufsize) *bufsize = ld->data[type].blob.size;
    return 0;
}

int __crystax_locale_loads(const char *encoding, const char *type, void **buf, size_t *bufsize)
{
    int ctype;

    if (type == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if (strcmp(type, "LC_CTYPE") == 0)
        ctype = LC_CTYPE;
    else if (strcmp(type, "LC_COLLATE") == 0)
        ctype = LC_COLLATE;
    else if (strcmp(type, "LC_MESSAGES") == 0)
        ctype = LC_MESSAGES;
    else if (strcmp(type, "LC_MONETARY") == 0)
        ctype = LC_MONETARY;
    else if (strcmp(type, "LC_NUMERIC") == 0)
        ctype = LC_NUMERIC;
    else if (strcmp(type, "LC_TIME") == 0)
        ctype = LC_TIME;
    else
    {
        errno = EINVAL;
        return -1;
    }

    return __crystax_locale_load(encoding, ctype, buf, bufsize);
}
