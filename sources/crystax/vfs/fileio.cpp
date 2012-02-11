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

#include "fileio/common.hpp"
#include "fileio/driver.hpp"
#include "system/driver.hpp"
#include "assets/driver.hpp"

namespace crystax
{
namespace fileio
{

bool volatile init_flag = false;
pthread_mutex_t init_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

const char *cpath = NULL;
size_t cpath_length = 0;
pthread_mutex_t cpath_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

struct fd_record_t
{
    DIR *dirp;
    int extfd;
    DIR *extdirp;
    driver_t *driver;
    const char *path;
};

fd_record_t fd_table[FD_TABLE_SIZE];
size_t fd_table_top = 0;
pthread_mutex_t fd_table_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

CRYSTAX_LOCAL
driver_t *load_driver(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data, driver_t *underlying)
{
    if (source != NULL && ::strcmp(source, "/dev/assets") == 0)
    {
        if (!jni::jvm())
        {
            ERR("ASSETS fstype requested but no JVM found");
            return NULL;
        }

        jobject context = (jobject)data;
        return new assets::driver_t(target, context, underlying);
    }

    ERR("unknown source %s and/or fstype %s", source, fstype);
    return NULL;
}

CRYSTAX_LOCAL
void unload_driver(driver_t *driver)
{
    delete driver;
}

void init_fd();

CRYSTAX_LOCAL
bool init_impl()
{
    DBG("initialization started");

    if (!system::driver_t::instance())
    {
        TRACE;
        return false;
    }

    TRACE;
    init_fd();

    DBG("initialization finished");
    return true;
}

CRYSTAX_LOCAL
bool init()
{
    TRACE;
    if (!init_flag)
    {
        TRACE;
        scope_lock_t lock(init_mutex);

        TRACE;
        if (!init_flag)
        {
            TRACE;
            if (!init_impl())
            {
                TRACE;
                return false;
            }
            TRACE;
            init_flag = true;
        }
    }
    TRACE;
    return true;
}

CRYSTAX_LOCAL
void init_fd()
{
    TRACE;
    scope_lock_t lock(fd_table_mutex);
    for (size_t fd = 0; fd != 3; ++fd)
    {
        fd_record_t &r = fd_table[fd];
        r.dirp = NULL;
        r.extfd = fd;
        r.extdirp = NULL;
        r.driver = system::driver_t::instance();
        r.path = NULL;
        if (fd_table_top < fd)
            fd_table_top = fd;
    }
    for (size_t fd = 3; fd != FD_TABLE_SIZE; ++fd)
    {
        fd_record_t &r = fd_table[fd];
        r.dirp = NULL;
        r.extfd = -1;
        r.extdirp = NULL;
        r.driver = NULL;
        r.path = NULL;
    }
}

CRYSTAX_LOCAL
int alloc_fd(const char *path, int extfd, driver_t *driver)
{
    scope_lock_t lock(fd_table_mutex);

    for (size_t fd = 0; fd != FD_TABLE_SIZE; ++fd)
    {
        fd_record_t &r = fd_table[fd];
        if (r.extfd < 0)
        {
            r.dirp = NULL;
            r.extfd = extfd;
            r.extdirp = NULL;
            r.driver = driver;
            r.path = absolutize(path);
            if (fd_table_top < fd)
                fd_table_top = fd;
            return fd;
        }
    }

    return -1;
}

CRYSTAX_LOCAL
void free_fd(int fd)
{
    if (fd < 0 || fd > FD_TABLE_SIZE)
        return;

    scope_lock_t lock(fd_table_mutex);

    fd_record_t &r = fd_table[fd];
    if (r.extfd < 0)
        return;

    r.dirp = NULL;
    r.extfd = -1;
    r.extdirp = NULL;
    r.driver = NULL;
    ::free((void*)r.path);
    r.path = NULL;

    if (fd_table_top == (size_t)fd)
    {
        for (size_t i = fd_table_top + 1; i > 0; --i)
        {
            if (fd_table[i - 1].extfd >= 0)
                break;
            --fd_table_top;
        }
    }
}

CRYSTAX_LOCAL
DIR *alloc_dirp(const char *path, DIR *extdirp, driver_t *driver)
{
    scope_lock_t lock(fd_table_mutex);

    for (size_t fd = 0; fd != FD_TABLE_SIZE; ++fd)
    {
        fd_record_t &r = fd_table[fd];
        if (r.extfd < 0)
        {
            r.dirp = reinterpret_cast<DIR*>(-(int)fd - 1);
            r.extfd = driver->dirfd(extdirp);
            r.extdirp = extdirp;
            r.driver = driver;
            r.path = absolutize(path);
            if (fd_table_top < fd)
                fd_table_top = fd;
            return r.dirp;
        }
    }

    return NULL;
}

CRYSTAX_LOCAL
void free_dirp(DIR *dirp)
{
    scope_lock_t lock(fd_table_mutex);

    for (size_t fd = 0; fd != fd_table_top; ++fd)
    {
        fd_record_t &r = fd_table[fd];
        if (r.dirp != dirp)
            continue;

        if (r.extfd < 0)
            break;

        r.dirp = NULL;
        r.extfd = -1;
        r.extdirp = NULL;
        r.driver = NULL;
        ::free((void*)r.path);
        r.path = NULL;

        if (fd_table_top == (size_t)fd)
        {
            for (size_t i = fd_table_top + 1; i > 0; --i)
            {
                if (fd_table[i - 1].extfd >= 0)
                    break;
                --fd_table_top;
            }
        }

        break;
    }
}

CRYSTAX_LOCAL
bool resolve(int fd, DIR **dirp, int *extfd, DIR **extdirp, driver_t **driver, path_t *path)
{
    if (fd < 0 || fd > FD_TABLE_SIZE)
    {
        errno = EBADFD;
        return false;
    }

    scope_lock_t lock(fd_table_mutex);

    fd_record_t const &r = fd_table[fd];
    if (dirp) *dirp = r.dirp;
    if (extfd) *extfd = r.extfd;
    if (extdirp) *extdirp = r.extdirp;
    if (driver) *driver = r.driver;
    if (path) path->reset(absolutize(r.path));

    return true;
}

CRYSTAX_LOCAL
bool resolve(DIR *dirp, int *fd, int *extfd, DIR **extdirp, driver_t **driver, path_t *path)
{
    scope_lock_t lock(fd_table_mutex);

    for (size_t i = 0; i != FD_TABLE_SIZE; ++i)
    {
        fd_record_t &r = fd_table[i];
        if (r.dirp != dirp)
            continue;

        if (fd) *fd = i;
        if (extfd) *extfd = r.extfd;
        if (extdirp) *extdirp = r.extdirp;
        if (driver) *driver = r.driver;
        if (path) path->reset(absolutize(r.path));
        return true;
    }

    return false;
}

} // namespace fileio
} // namespace crystax

int __crystax_fileio_init()
{
    return ::crystax::fileio::init() ? 0 : -1;
}
