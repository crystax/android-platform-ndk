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

#include "fileio/api.hpp"

namespace crystax
{
namespace fileio
{

#if defined(CRYSTAX_DEBUG) && CRYSTAX_DEBUG == 1
static const char *mode_s(int oflag)
{
    switch (oflag)
    {
    case O_RDONLY: return "O_RDONLY";
    case O_WRONLY: return "O_WRONLY";
    case O_RDWR:   return "O_RDWR";
    default:       return "UNKNOWN";
    }
}
#endif

CRYSTAX_LOCAL
int open(const char *path, int oflag, va_list &vl)
{
    DBG("path=%s, oflag=%d (%s)", path, oflag, mode_s(oflag));

    driver_t *driver = find_driver(path);
    if (!driver)
        return -1;

    DBG("open with driver %s", driver->name());
    int extfd = driver->open(path, oflag, vl);
    DBG("extfd=%d", extfd);

    if (extfd == -1)
        return -1;

    int fd = alloc_fd(path, extfd, driver);
    if (fd < 0)
    {
        ERR("can't alloc fd, return -1");
        driver->close(extfd);
        errno = EMFILE;
        return -1;
    }

    DBG("return fd=%d", fd);
    return fd;
}

} // namespace fileio
} // namespace crystax

CRYSTAX_GLOBAL
int open(const char *path, int oflag, ...)
{
    va_list vl;
    va_start(vl, oflag);
    int ret = ::crystax::fileio::open(path, oflag, vl);
    va_end(vl);
    return ret;
}

CRYSTAX_GLOBAL
int creat(const char* path, mode_t mode)
{
    return ::open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
}
