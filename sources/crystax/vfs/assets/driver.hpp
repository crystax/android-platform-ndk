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

#ifndef _CRYSTAX_FILEIO_ASSETS_DRIVER_HPP_ab9b561c8d5944db8adb1654680308cd
#define _CRYSTAX_FILEIO_ASSETS_DRIVER_HPP_ab9b561c8d5944db8adb1654680308cd

#include <crystax/list.hpp>
#include "fileio/driver.hpp"

namespace crystax
{
namespace fileio
{
namespace assets
{

class driver_t : public ::crystax::fileio::driver_t
{
public:
    driver_t(const char *root, jobject context, fileio::driver_t *d);
    ~driver_t();

    const char *name() const {return "ASSETS";}
    const char *info() const {return root().c_str();}

    int    chown(const char *path, uid_t uid, gid_t gid);
    int    close(int fd);
    int    closedir(DIR *dirp);
    int    dirfd(DIR *dirp);
    int    dup(int fd);
    int    dup2(int fd, int fd2);
    int    fchown(int fd, uid_t uid, gid_t gid);
    int    fcntl(int fd, int command, va_list &vl);
    int    fdatasync(int fd);
    DIR *  fdopendir(int fd);
    int    flock(int fd, int operation);
    int    fstat(int fd, struct stat *st);
    int    fsync(int fd);
    int    ftruncate(int fd, off_t offset);
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

private:
    void init_jni(JNIEnv *env, jni::jhobject const &objContext);
    bool check_subpath(abspath_t const &abspath);

    int stat_as(path_t const &rpath, struct stat *st);
    int stat_ul(abspath_t const &abspath, path_t const &rpath, struct stat *st);

    int mkdir_p(abspath_t const &abspath, mode_t mode);
    bool copy_from_assets(abspath_t const &abspath, path_t const &rpath);

    void init_fd();
    int alloc_fd(jobject obj, size_t size, abspath_t const &abspath);
    int alloc_fd(int extfd, abspath_t const &abspath);
    void free_fd(int fd);
    bool resolve(int fd, jobject *obj, size_t *pos, size_t *size, int *extfd, abspath_t *abspath);
    bool update(int fd, size_t pos);

    void load_metadata();
    void save_metadata();
    bool read_metadata_entry(int fd, abspath_t *abspath, bool *removed);
    bool write_metadata_entry(int fd, abspath_t const &abspath, bool removed);

private:
    jobject objAssetManager;

    jmethodID midAmOpen;
    jmethodID midAmList;
    jmethodID midIsClose;
    jmethodID midIsAvail;
    jmethodID midIsRead;
    jmethodID midIsSkip;
    jmethodID midIsMark;
    jmethodID midIsReset;

    struct stat sst;

    struct fd_entry_t
    {
        jobject obj;
        size_t pos;
        size_t size;
        int extfd;
        abspath_t path;
    };

    fd_entry_t fd_table[FD_TABLE_SIZE];
    pthread_mutex_t fd_table_mutex;

    struct metadata_entry_t
    {
        abspath_t path;
        bool removed;

        metadata_entry_t *next;
        metadata_entry_t *prev;

        metadata_entry_t(const char *p, bool r)
            :path(p), removed(r), next(0), prev(0)
        {}
    };

    list_t<metadata_entry_t> metadata;
    pthread_mutex_t metadata_mutex;

    static bool metadata_comparator(metadata_entry_t const &e, const char *path);
};

} // namespace assets
} // namespace fileio
} // namespace crystax

#endif // _CRYSTAX_FILEIO_ASSETS_DRIVER_HPP_ab9b561c8d5944db8adb1654680308cd
