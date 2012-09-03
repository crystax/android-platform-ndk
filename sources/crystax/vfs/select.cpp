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

CRYSTAX_LOCAL
int select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv)
{
    DBG("maxfd=%d, rfd=%p, wfd=%p, efd=%p, tv=%p", maxfd, rfd, wfd, efd, tv);

    driver_t *driver = NULL;
    int extmaxfd = 0;
    fd_set extrfd;
    fd_set extwfd;
    fd_set extefd;

    if (rfd) FD_ZERO(&extrfd);
    if (wfd) FD_ZERO(&extwfd);
    if (efd) FD_ZERO(&extefd);

    for (int fd = 0; fd < maxfd; ++fd)
    {
#define CRYSTAX_SELECT_SET(fd, fds, extfds)                        \
        if (fds && FD_ISSET(fd, fds))                              \
        {                                                          \
            int extfd;                                             \
            driver_t *d;                                           \
            if (!resolve(fd, NULL, &extfd, NULL, &d))              \
                return -1;                                         \
            if (!driver)                                           \
                driver = d;                                        \
            else if (driver != d)                                  \
            {                                                      \
                ERR("select across sources not supported yet");    \
                errno = EXDEV;                                     \
                return -1;                                         \
            }                                                      \
            if (extmaxfd <= extfd)                                 \
                extmaxfd = extfd + 1;                              \
            FD_SET(extfd, &extfds);                                \
        }
        CRYSTAX_SELECT_SET(fd, rfd, extrfd);
        CRYSTAX_SELECT_SET(fd, wfd, extwfd);
        CRYSTAX_SELECT_SET(fd, efd, extefd);
#undef CRYSTAX_SELECT_SET
    }
    if (!driver)
    {
        DBG("empty sets");
        return 0;
    }

    return driver->select(extmaxfd, rfd ? &extrfd : NULL, wfd ? &extwfd : NULL,
        efd ? &extefd : NULL, tv);
}

} // namespace fileio
} // namespace crystax

CRYSTAX_GLOBAL
int select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv)
{
    return ::crystax::fileio::select(maxfd, rfd, wfd, efd, tv);
}
