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

#include <crystax.h>

#include "fileio/common.hpp"
#include "fileio/driver.hpp"

#include "crystax/memory.hpp"
#include "crystax/lock.hpp"

#include "fileio/system/driver.hpp"

namespace crystax
{
namespace fileio
{

driver_t *load_driver(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data, driver_t *underlying);
void unload_driver(driver_t *driver);

enum
{
    MOUNT_TABLE_SIZE = 256
};

static driver_t *mount_table[MOUNT_TABLE_SIZE];
static int mount_table_pos = 0;
static pthread_mutex_t mount_table_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

CRYSTAX_LOCAL
driver_t *find_driver(const char *path)
{
    DBG("path=%s", path);

    if (path == NULL || *path == '\0')
        return NULL;

    scope_lock_t lock(mount_table_mutex);

    if (mount_table_pos == 0)
    {
        DBG("path=%s: no mount records registered, use SYSTEM driver", path);
        return system::driver_t::instance();
    }

    abspath_t abspath(path);
    size_t nlen = ::strlen(abspath.c_str());
    for (size_t i = mount_table_pos; i > 0; --i)
    {
        driver_t *d = mount_table[i - 1];
        size_t len = ::strlen(d->root().c_str());
        if (nlen >= len && ::strncmp(abspath.c_str(), d->root().c_str(), len) == 0)
        {
            DBG("path=%s: use driver %s (%s)", path, d->name(), d->info());
            return d;
        }
    }

    DBG("path=%s: no mount record found, use system driver", path);
    return system::driver_t::instance();
}

CRYSTAX_LOCAL
int mount(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data)
{
    DBG("source=%s, target=%s, fstype=%s, flags=%lu, data=%p",
        source, target, fstype, flags, data);

    scope_lock_t lock(mount_table_mutex);

    if (mount_table_pos >= MOUNT_TABLE_SIZE)
    {
        ERR("too much mount calls performed (max %d)", (int)MOUNT_TABLE_SIZE);
        errno = ENFILE;
        return -1;
    }

    abspath_t abspath(target);

    // Found underlying driver
    driver_t *underlying = system::driver_t::instance();
    for (size_t i = mount_table_pos; i > 0; --i)
    {
        driver_t *prev = mount_table[i - 1];
        if (abspath.subpath(prev->root()))
        {
            underlying = prev;
            break;
        }
    }

    driver_t *driver = load_driver(source, abspath.c_str(), fstype, flags, data, underlying);
    if (!driver)
        return system_mount(source, target, fstype, flags, data);

    DBG("load driver: %s (%s), underlying: %s (%s)", driver->name(), driver->info(), underlying->name(), underlying->info());

    mount_table[mount_table_pos] = driver;

    ++mount_table_pos;

    return 0;
}

CRYSTAX_LOCAL
int umount(const char *target, int flags)
{
    DBG("target=%s, flags=%d", target, flags);

    if (target == NULL || *target == '\0')
    {
        ERR("empty target");
        errno = EINVAL;
        return -1;
    }

    scope_lock_t lock(mount_table_mutex);

    abspath_t abspath(target);
    for (size_t i = mount_table_pos; i > 0; --i)
    {
        driver_t *d = mount_table[i - 1];
        if (d->root().length() > abspath.length() && d->root().subpath(abspath))
        {
            ERR("unmount failed: %s busy", abspath.c_str());
            errno = EBUSY;
            return -1;
        }
        if (d->root() == abspath)
        {
            DBG("unmount target %s", abspath.c_str());
            unload_driver(d);
            // Shift above records
            for (size_t j = i - 1; j < (size_t)mount_table_pos - 1; ++j)
                mount_table[j] = mount_table[j + 1];
            --mount_table_pos;
            return 0;
        }
    }

    return system_umount2(target, flags);
}

} // namespace fileio
} // namespace crystax

CRYSTAX_GLOBAL
int mount(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data)
{
    return ::crystax::fileio::mount(source, target, fstype, flags, data);
}

CRYSTAX_GLOBAL
int umount(const char *target)
{
    return ::crystax::fileio::umount(target, 0);
}

CRYSTAX_GLOBAL
int umount2(const char *target, int flags)
{
    return ::crystax::fileio::umount(target, flags);
}
