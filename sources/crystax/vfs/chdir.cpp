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

extern const char *cpath;
extern size_t cpath_length;
extern pthread_mutex_t cpath_mutex;

CRYSTAX_LOCAL
int chdir(const char *path)
{
    DBG("path=%s", path);

    abspath_t abspath(path);

    driver_t *driver = find_driver(abspath.c_str());

    DBG("path=%s: get driver's stat", abspath.c_str());
    struct stat st;
    if (driver->stat(abspath.c_str(), &st) == -1)
        return -1;

    if ((st.st_mode & S_IFDIR) == 0)
    {
        ERR("path=%s: driver's stat reported this is not dir", abspath.c_str());
        errno = ENOTDIR;
        return -1;
    }

    scope_lock_t lock(cpath_mutex);

    // Call system_chdir but ignore it's return value
    system_chdir(abspath.c_str());

    DBG("chdir to %s", abspath.c_str());
    ::free((void*)cpath);
    cpath_length = abspath.length();
    cpath = abspath.release();
    return 0;
}

} // namespace fileio
} // namespace crystax

CRYSTAX_GLOBAL
int chdir(const char *path)
{
    return ::crystax::fileio::chdir(path);
}
