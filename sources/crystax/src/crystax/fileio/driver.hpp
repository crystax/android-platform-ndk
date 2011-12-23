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

#ifndef _CRYSTAX_FILEIO_DRIVER_HPP_acf1e17ac4114b9a8387b40660a8fc61
#define _CRYSTAX_FILEIO_DRIVER_HPP_acf1e17ac4114b9a8387b40660a8fc61

#include "fileio/common.hpp"

namespace crystax
{
namespace fileio
{

class driver_t : non_copyable_t
{
public:
    driver_t(const char *root, driver_t *d)
        :rootpath(root), underlying_driver(d)
    {}

    virtual const char *name() const = 0;
    virtual const char *info() const = 0;

    path_t const &root() const {return rootpath;}

    driver_t *underlying() const {return underlying_driver;}

    virtual int    chown(const char *path, uid_t uid, gid_t gid) = 0;
    virtual int    close(int fd) = 0;
    virtual int    closedir(DIR *dirp) = 0;
    virtual int    dirfd(DIR *dirp) = 0;
    virtual int    dup(int fd) = 0;
    virtual int    dup2(int fd, int fd2) = 0;
    virtual int    fchown(int fd, uid_t uid, gid_t gid) = 0;
    virtual int    fcntl(int fd, int command, va_list &vl) = 0;
    virtual int    fdatasync(int fd) = 0;
    virtual DIR *  fdopendir(int fd) = 0;
    virtual int    flock(int fd, int operation) = 0;
    virtual int    fstat(int fd, struct stat *st) = 0;
    virtual int    fsync(int fd) = 0;
    virtual int    ftruncate(int fd, off_t offset) = 0;
    virtual int    getdents(unsigned int fd, struct dirent *entry, unsigned int count) = 0;
    virtual int    ioctl(int fd, int request, va_list &vl) = 0;
    virtual int    lchown(const char *path, uid_t uid, gid_t gid) = 0;
    virtual int    link(const char *src, const char *dst) = 0;
    virtual off_t  lseek(int fd, off_t offset, int whence) = 0;
    virtual loff_t lseek64(int fd, loff_t offset, int whence) = 0;
    virtual int    lstat(const char *path, struct stat *st) = 0;
    virtual int    mkdir(const char *path, mode_t mode) = 0;
    virtual int    open(const char *path, int oflag, va_list &vl) = 0;
    virtual DIR *  opendir(const char *dirpath) = 0;
    virtual ssize_t pread(int fd, void *buf, size_t count, off_t offset) = 0;
    virtual ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) = 0;
    virtual ssize_t read(int fd, void *buf, size_t count) = 0;
    virtual int    readv(int fd, const struct iovec *iov, int count) = 0;
    virtual struct dirent *readdir(DIR *dirp) = 0;
    virtual int    readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result) = 0;
    virtual int    readlink(const char *path, char *buf, size_t bufsize) = 0;
    virtual int    remove(const char *path) = 0;
    virtual int    rename(const char *oldpath, const char *newpath) = 0;
    virtual void   rewinddir(DIR *dirp) = 0;
    virtual int    rmdir(const char *path) = 0;
    virtual int    scandir(const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *),
        int (*compar)(const struct dirent **, const struct dirent **)) = 0;
    virtual void   seekdir(DIR *dirp, long offset) = 0;
    virtual int    select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv) = 0;
    virtual int    stat(const char *path, struct stat *st) = 0;
    virtual int    symlink(const char *src, const char *dst) = 0;
    virtual long   telldir(DIR *dirp) = 0;
    virtual int    unlink(const char *path) = 0;
    virtual ssize_t write(int fd, const void *buf, size_t count) = 0;
    virtual int    writev(int fd, const struct iovec *iov, int count) = 0;

    int open(const char *path, int oflag, ...)
    {
        va_list vl;
        va_start(vl, oflag);
        int ret = this->open(path, oflag, vl);
        va_end(vl);
        return ret;
    }

    int fcntl(int fd, int command, ...)
    {
        va_list vl;
        va_start(vl, command);
        int ret = this->fcntl(fd, command, vl);
        va_end(vl);
        return ret;
    }

    int ioctl(int fd, int request, ...)
    {
        va_list vl;
        va_start(vl, request);
        int ret = this->ioctl(fd, request, vl);
        va_end(vl);
        return ret;
    }

private:
    path_t rootpath;
    driver_t *underlying_driver;
};

driver_t *find_driver(const char *path);
bool resolve(int fd, DIR **dirp = 0, int *extfd = 0, DIR **extdirp = 0, driver_t **driver = 0, path_t *path = 0);
bool resolve(DIR *dirp, int *fd = 0, int *extfd = 0, DIR **extdirp = 0, driver_t **driver = 0, path_t *path = 0);

} // namespace fileio
} // namespace crystax

#endif // _CRYSTAX_FILEIO_DRIVER_HPP_acf1e17ac4114b9a8387b40660a8fc61
