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

#ifndef _CRYSTAX_FILEIO_SYSTEM_DRIVER_HPP_8c3ea337310344549cb60cbfe3683166
#define _CRYSTAX_FILEIO_SYSTEM_DRIVER_HPP_8c3ea337310344549cb60cbfe3683166

#include "fileio/driver.hpp"

namespace crystax
{
namespace fileio
{
namespace system
{

class driver_t : public ::crystax::fileio::driver_t
{
private:
    driver_t();
    ~driver_t();

public:
    static driver_t *instance();

    const char *name() const {return "SYSTEM";}
    const char *info() const {return "ANDROID";}

    int    chdir(const char *path);
    int    chown(const char *path, uid_t uid, gid_t gid);
    int    chroot(const char *path);
    int    close(int fd);
    int    closedir(DIR *dirp);
    int    dirfd(DIR *dirp);
    int    dup(int fd);
    int    dup2(int fd, int fd2);
    int    fchown(int fd, uid_t uid, gid_t gid);
    int    fchdir(int fd);
    int    fcntl(int fd, int command, va_list &vl);
    int    fdatasync(int fd);
    DIR *  fdopendir(int fd);
    int    flock(int fd, int operation);
    int    fstat(int fd, struct stat *st);
    int    fsync(int fd);
    int    ftruncate(int fd, off_t offset);
    char * getcwd(char *buf, size_t size);
    int    getdents(unsigned int fd, struct dirent *entry, unsigned int count);
    int    ioctl(int fd, int request, va_list &vl);
    int    lchown(const char *path, uid_t uid, gid_t gid);
    int    link(const char *src, const char *dst);
    off_t  lseek(int fd, off_t offset, int whence);
    loff_t lseek64(int fd, loff_t offset, int whence);
    int    lstat(const char *path, struct stat *st);
    int    mkdir(const char *path, mode_t mode);
    int    open(const char *path, int oflag, va_list &vl);
    DIR *  opendir(const char *dirpath);
    ssize_t pread(int fd, void *buf, size_t count, off_t offset);
    ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
    ssize_t read(int fd, void *buf, size_t count);
    int    readv(int fd, const struct iovec *iov, int count);
    struct dirent *readdir(DIR *dirp);
    int    readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
    int    readlink(const char *path, char *buf, size_t bufsize);
    int    remove(const char *path);
    int    rename(const char *oldpath, const char *newpath);
    void   rewinddir(DIR *dirp);
    int    rmdir(const char *path);
    int    scandir(const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *),
        int (*compar)(const struct dirent **, const struct dirent **));
    void   seekdir(DIR *dirp, long offset);
    int    select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv);
    int    stat(const char *path, struct stat *st);
    int    symlink(const char *src, const char *dst);
    long   telldir(DIR *dirp);
    int    unlink(const char *path);
    ssize_t write(int fd, const void *buf, size_t count);
    int    writev(int fd, const struct iovec *iov, int count);
};

} // namespace system
} // namespace fileio
} // namespace crystax

#endif // _CRYSTAX_FILEIO_SYSTEM_DRIVER_HPP_8c3ea337310344549cb60cbfe3683166
