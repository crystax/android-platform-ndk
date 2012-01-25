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

#include "fileio/system/driver.hpp"

#include <new>

#ifdef MODULE_INIT
#undef MODULE_INIT
#endif
#define MODULE_INIT \
    if (!::crystax::fileio::system::init()) \
        ::abort()

namespace crystax
{

namespace fileio
{
namespace system
{

bool volatile init_flag = false;
pthread_mutex_t init_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

typedef int (*func_accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*func_access_t)(const char *path, int mode);
typedef int (*func_bind_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*func_chdir_t)(const char *path);
typedef int (*func_chown_t)(const char *path, uid_t uid, gid_t gid);
typedef int (*func_chroot_t)(const char *path);
typedef int (*func_close_t)(int fd);
typedef int (*func_closedir_t)(DIR *dirp);
typedef int (*func_connect_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*func_dirfd_t)(DIR *dirp);
typedef int (*func_dup2_t)(int fd, int fd2);
typedef int (*func_dup_t)(int fd);
typedef int (*func_fchdir_t)(int fd);
typedef int (*func_fchown_t)(int fd, uid_t uid, gid_t gid);
typedef int (*func_fcntl_t)(int fd, int command, ...);
typedef int (*func_fdatasync_t)(int fd);
typedef DIR *(*func_fdopendir_t)(int fd);
typedef int (*func_flock_t)(int fd, int operation);
typedef int (*func_fstat_t)(int filedes, struct stat *buf);
typedef int (*func_fsync_t)(int fd);
typedef int (*func_ftruncate_t)(int fd, off_t offset);
typedef char *(*func_getcwd_t)(char *buf, size_t size);
typedef int (*func_getdents_t)(unsigned int, struct dirent *, unsigned int);
typedef int (*func_getpwnam_r_t)(const char*, struct passwd*, char*, size_t, struct passwd**);
typedef int (*func_getpwuid_r_t)(uid_t, struct passwd*, char*, size_t, struct passwd**);
typedef int (*func_getsockname_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*func_getsockopt_t)(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
typedef int (*func_ioctl_t)(int fd, int request, ...);
typedef int (*func_lchown_t)(const char *path, uid_t uid, gid_t gid);
typedef int (*func_link_t)(const char *src, const char *dst);
typedef int (*func_listen_t)(int sockfd, int backlog);
typedef off_t (*func_lseek_t)(int fd, off_t offset, int whence);
typedef loff_t (*func_lseek64_t)(int fd, loff_t offset, int whence);
typedef int (*func_lstat_t)(const char *path, struct stat *buf);
typedef int (*func_mkdir_t)(const char *path, mode_t mode);
typedef int (*func_mmap_t)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
typedef int (*func_mount_t)(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data);
typedef int (*func_open_t)(const char *path, int oflag, ...);
typedef DIR *(*func_opendir_t)(const char *dirpath);
typedef int (*func_pipe_t)(int pipefd[2]);
typedef ssize_t (*func_pread_t)(int fd, void *buf, size_t count, off_t offset);
typedef ssize_t (*func_pwrite_t)(int fd, const void *buf, size_t count, off_t offset);
typedef int (*func_pthread_create_t)(pthread_t *pth, pthread_attr_t const *pattr, void * (*func)(void *), void *arg);
typedef ssize_t (*func_read_t)(int fd, void *buf, size_t count);
typedef int (*func_readv_t)(int fd, const struct iovec *iov, int count);
typedef struct dirent *(*func_readdir_t)(DIR *dirp);
typedef int (*func_readdir_r_t)(DIR *dirp, struct dirent *entry, struct dirent **result);
typedef int (*func_readlink_t)(const char *path, char *buf, size_t bufsize);
typedef ssize_t (*func_recv_t)(int sockfd, void *buf, size_t len, unsigned int flags);
typedef ssize_t (*func_recvfrom_t)(int sockfd, void *buf, size_t len, unsigned int flags,
    const struct sockaddr *addr, socklen_t *addrlen);
typedef int (*func_recvmsg_t)(int sockfd, struct msghdr *msg, unsigned int flags);
typedef int (*func_remove_t)(const char *path);
typedef int (*func_rename_t)(const char *oldpath, const char *newpath);
typedef void (*func_rewinddir_t)(DIR *dirp);
typedef int (*func_rmdir_t)(const char *path);
typedef int (*func_scandir_t)(const char *dir, struct dirent ***namelist,
    int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **));
typedef void (*func_seekdir_t)(DIR *dirp, long offset);
typedef int (*func_select_t)(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv);
typedef ssize_t (*func_send_t)(int sockfd, const void *buf, size_t len, unsigned int flags);
typedef int (*func_sendmsg_t)(int sockfd, const struct msghdr *msg, unsigned int flags);
typedef ssize_t (*func_sendto_t)(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *addr, socklen_t addrlen);
typedef int (*func_setsockopt_t)(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
typedef int (*func_shutdown_t)(int sockfd, int how);
typedef int (*func_socket_t)(int domain, int type, int protocol);
typedef int (*func_stat_t)(const char *path, struct stat *buf);
typedef int (*func_symlink_t)(const char *src, const char *dst);
typedef long (*func_telldir_t)(DIR *dirp);
typedef int (*func_umount_t)(const char *target);
typedef int (*func_umount2_t)(const char *target, int flags);
typedef int (*func_unlink_t)(const char *path);
typedef ssize_t (*func_write_t)(int fd, const void *buf, size_t count);
typedef int (*func_writev_t)(int fd, const struct iovec *iov, int count);

func_accept_t func_accept = NULL;
func_access_t func_access = NULL;
func_bind_t func_bind = NULL;
func_chdir_t func_chdir = NULL;
func_chown_t func_chown = NULL;
func_chroot_t func_chroot = NULL;
func_close_t func_close = NULL;
func_closedir_t func_closedir = NULL;
func_connect_t func_connect = NULL;
func_dirfd_t func_dirfd = NULL;
func_dup2_t func_dup2 = NULL;
func_dup_t func_dup = NULL;
func_fchdir_t func_fchdir = NULL;
func_fchown_t func_fchown = NULL;
func_fcntl_t func_fcntl = NULL;
func_fdatasync_t func_fdatasync = NULL;
func_fdopendir_t func_fdopendir = NULL;
func_flock_t func_flock = NULL;
func_fstat_t func_fstat = NULL;
func_fsync_t func_fsync = NULL;
func_ftruncate_t func_ftruncate = NULL;
func_getcwd_t func_getcwd = NULL;
func_getdents_t func_getdents = NULL;
func_getpwnam_r_t func_getpwnam_r = NULL;
func_getpwuid_r_t func_getpwuid_r = NULL;
func_getsockname_t func_getsockname = NULL;
func_getsockopt_t func_getsockopt = NULL;
func_ioctl_t func_ioctl = NULL;
func_lchown_t func_lchown = NULL;
func_link_t func_link = NULL;
func_listen_t func_listen = NULL;
func_lseek_t func_lseek = NULL;
func_lseek64_t func_lseek64 = NULL;
func_lstat_t func_lstat = NULL;
func_mkdir_t func_mkdir = NULL;
func_mmap_t func_mmap = NULL;
func_mount_t func_mount = NULL;
func_open_t func_open = NULL;
func_opendir_t func_opendir = NULL;
func_pipe_t func_pipe = NULL;
func_pread_t func_pread = NULL;
func_pwrite_t func_pwrite = NULL;
func_pthread_create_t func_pthread_create = NULL;
func_read_t func_read = NULL;
func_readv_t func_readv = NULL;
func_readdir_r_t func_readdir_r = NULL;
func_readdir_t func_readdir = NULL;
func_readlink_t func_readlink = NULL;
func_recv_t func_recv = NULL;
func_recvfrom_t func_recvfrom = NULL;
func_recvmsg_t func_recvmsg = NULL;
func_remove_t func_remove = NULL;
func_rename_t func_rename = NULL;
func_rewinddir_t func_rewinddir = NULL;
func_rmdir_t func_rmdir = NULL;
func_scandir_t func_scandir = NULL;
func_seekdir_t func_seekdir = NULL;
func_select_t func_select = NULL;
func_send_t func_send = NULL;
func_sendmsg_t func_sendmsg = NULL;
func_sendto_t func_sendto = NULL;
func_setsockopt_t func_setsockopt = NULL;
func_shutdown_t func_shutdown = NULL;
func_socket_t func_socket = NULL;
func_stat_t func_stat = NULL;
func_symlink_t func_symlink = NULL;
func_telldir_t func_telldir = NULL;
func_unlink_t func_unlink = NULL;
func_umount_t func_umount = NULL;
func_umount2_t func_umount2 = NULL;
func_write_t func_write = NULL;
func_writev_t func_writev = NULL;

CRYSTAX_LOCAL
bool init_impl()
{
    TRACE;

    void *pc = ::dlopen("/system/lib/libc.so", RTLD_LAZY);
    TRACE;
    if (pc == NULL)
    {
        DBG("dlopen() returned NULL");
        return false;
    }

#define CRYSTAX_LOAD_SYMBOL(x) \
    TRACE; \
    func_ ## x = (func_ ## x ## _t)dlsym(pc, #x); \
    if (func_ ## x == NULL) \
    { \
        DBG("dlsym(\"" #x "\") returned NULL"); \
        return false; \
    }

    CRYSTAX_LOAD_SYMBOL(accept);
    CRYSTAX_LOAD_SYMBOL(access);
    CRYSTAX_LOAD_SYMBOL(bind);
    CRYSTAX_LOAD_SYMBOL(chdir);
    CRYSTAX_LOAD_SYMBOL(chown);
    CRYSTAX_LOAD_SYMBOL(chroot);
    CRYSTAX_LOAD_SYMBOL(close);
    CRYSTAX_LOAD_SYMBOL(closedir);
    CRYSTAX_LOAD_SYMBOL(connect);
    CRYSTAX_LOAD_SYMBOL(dirfd);
    CRYSTAX_LOAD_SYMBOL(dup);
    CRYSTAX_LOAD_SYMBOL(dup2);
    CRYSTAX_LOAD_SYMBOL(fchdir);
    CRYSTAX_LOAD_SYMBOL(fchown);
    CRYSTAX_LOAD_SYMBOL(fcntl);
    CRYSTAX_LOAD_SYMBOL(fdopendir);
    CRYSTAX_LOAD_SYMBOL(flock);
    CRYSTAX_LOAD_SYMBOL(fstat);
    CRYSTAX_LOAD_SYMBOL(fsync);
    CRYSTAX_LOAD_SYMBOL(ftruncate);
    CRYSTAX_LOAD_SYMBOL(getcwd);
    CRYSTAX_LOAD_SYMBOL(getdents);
    CRYSTAX_LOAD_SYMBOL(getsockname);
    CRYSTAX_LOAD_SYMBOL(getsockopt);
    CRYSTAX_LOAD_SYMBOL(ioctl);
    CRYSTAX_LOAD_SYMBOL(lchown);
    CRYSTAX_LOAD_SYMBOL(link);
    CRYSTAX_LOAD_SYMBOL(listen);
    CRYSTAX_LOAD_SYMBOL(lseek);
    CRYSTAX_LOAD_SYMBOL(lseek64);
    CRYSTAX_LOAD_SYMBOL(lstat);
    CRYSTAX_LOAD_SYMBOL(mkdir);
    CRYSTAX_LOAD_SYMBOL(mmap);
    CRYSTAX_LOAD_SYMBOL(mount);
    CRYSTAX_LOAD_SYMBOL(open);
    CRYSTAX_LOAD_SYMBOL(opendir);
    CRYSTAX_LOAD_SYMBOL(pipe);
    CRYSTAX_LOAD_SYMBOL(pread);
    CRYSTAX_LOAD_SYMBOL(pwrite);
    CRYSTAX_LOAD_SYMBOL(pthread_create);
    CRYSTAX_LOAD_SYMBOL(read);
    CRYSTAX_LOAD_SYMBOL(readv);
    CRYSTAX_LOAD_SYMBOL(readdir);
    CRYSTAX_LOAD_SYMBOL(readdir_r);
    CRYSTAX_LOAD_SYMBOL(readlink);
    CRYSTAX_LOAD_SYMBOL(recv);
    CRYSTAX_LOAD_SYMBOL(recvfrom);
    CRYSTAX_LOAD_SYMBOL(recvmsg);
    CRYSTAX_LOAD_SYMBOL(remove);
    CRYSTAX_LOAD_SYMBOL(rename);
    CRYSTAX_LOAD_SYMBOL(rewinddir);
    CRYSTAX_LOAD_SYMBOL(rmdir);
    CRYSTAX_LOAD_SYMBOL(scandir);
    CRYSTAX_LOAD_SYMBOL(select);
    CRYSTAX_LOAD_SYMBOL(send);
    CRYSTAX_LOAD_SYMBOL(sendmsg);
    CRYSTAX_LOAD_SYMBOL(sendto);
    CRYSTAX_LOAD_SYMBOL(setsockopt);
    CRYSTAX_LOAD_SYMBOL(shutdown);
    CRYSTAX_LOAD_SYMBOL(socket);
    CRYSTAX_LOAD_SYMBOL(stat);
    CRYSTAX_LOAD_SYMBOL(symlink);
    CRYSTAX_LOAD_SYMBOL(umount);
    CRYSTAX_LOAD_SYMBOL(umount2);
    CRYSTAX_LOAD_SYMBOL(unlink);
    CRYSTAX_LOAD_SYMBOL(write);
    CRYSTAX_LOAD_SYMBOL(writev);

    TRACE;
    func_fdatasync = (func_fdatasync_t)dlsym(pc, "fdatasync");
    /* Android 2.1 have no fdatasync call. Use fsync instead */
    if (func_fdatasync == NULL)
        func_fdatasync = func_fsync;

    TRACE;
    /* Don't check return value; there is no such functions in Bionic currently,
       but maybe in future they'll appear, so then we'll use native implementations
    */
    func_seekdir = (func_seekdir_t)dlsym(pc, "seekdir");
    func_telldir = (func_telldir_t)dlsym(pc, "telldir");

    func_getpwnam_r = (func_getpwnam_r_t)dlsym(pc, "getpwnam_r");
    func_getpwuid_r = (func_getpwuid_r_t)dlsym(pc, "getpwuid_r");

#undef CRYSTAX_LOAD_SYMBOL
    TRACE;
    dlclose(pc);

    TRACE;
    return true;
}

CRYSTAX_LOCAL
bool init()
{
    if (!init_flag)
    {
        scope_lock_t lock(init_mutex);

        if (!init_flag)
        {
            if (!init_impl())
                return false;
            init_flag = true;
        }
    }
    return true;
}

CRYSTAX_LOCAL
driver_t *driver_t::instance()
{
    static driver_t volatile *obj = NULL;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    if (!obj)
    {
        scope_lock_t guard(lock);
        if (!obj)
        {
            // Allocate memory and ensure it's aligned to int bounds
            const size_t align = sizeof(int);
            uint8_t *buf = static_cast<uint8_t *>(::malloc(sizeof(driver_t) + align));
            unsigned v = reinterpret_cast<uint64_t>(buf) % align;
            if (v != 0)
                buf += align - v;
            // Then, create object in-place on top of allocated memory
            obj = new (buf) driver_t();
        }
    }
    return const_cast<driver_t *>(obj);
}

CRYSTAX_LOCAL
driver_t::driver_t()
    :fileio::driver_t("/", NULL)
{
    TRACE;
}

CRYSTAX_LOCAL
driver_t::~driver_t()
{
    TRACE;
}

CRYSTAX_LOCAL
int driver_t::chdir(const char *path)
{
    return system_chdir(path);
}

CRYSTAX_LOCAL
int driver_t::chown(const char *path, uid_t uid, gid_t gid)
{
    return system_chown(path, uid, gid);
}

CRYSTAX_LOCAL
int driver_t::chroot(const char *path)
{
    return system_chroot(path);
}

CRYSTAX_LOCAL
int driver_t::close(int fd)
{
    return system_close(fd);
}

CRYSTAX_LOCAL
int driver_t::closedir(DIR *dirp)
{
    return system_closedir(dirp);
}

CRYSTAX_LOCAL
int driver_t::dirfd(DIR *dirp)
{
    return system_dirfd(dirp);
}

CRYSTAX_LOCAL
int driver_t::dup(int fd)
{
    return system_dup(fd);
}

CRYSTAX_LOCAL
int driver_t::dup2(int fd, int fd2)
{
    return system_dup2(fd, fd2);
}

CRYSTAX_LOCAL
int driver_t::fchdir(int fd)
{
    return system_fchdir(fd);
}

CRYSTAX_LOCAL
int driver_t::fchown(int fd, uid_t uid, gid_t gid)
{
    return system_fchown(fd, uid, gid);
}

CRYSTAX_LOCAL
int driver_t::fcntl(int fd, int command, va_list &vl)
{
    return system_fcntl_v(fd, command, vl);
}

CRYSTAX_LOCAL
int driver_t::fdatasync(int fd)
{
    return system_fdatasync(fd);
}

CRYSTAX_LOCAL
DIR *driver_t::fdopendir(int fd)
{
    return system_fdopendir(fd);
}

CRYSTAX_LOCAL
int driver_t::flock(int fd, int operation)
{
    return system_flock(fd, operation);
}

CRYSTAX_LOCAL
int driver_t::fstat(int fd, struct stat *st)
{
    return system_fstat(fd, st);
}

CRYSTAX_LOCAL
int driver_t::fsync(int fd)
{
    return system_fsync(fd);
}

CRYSTAX_LOCAL
int driver_t::ftruncate(int fd, off_t offset)
{
    return system_ftruncate(fd, offset);
}

CRYSTAX_LOCAL
char *driver_t::getcwd(char *buf, size_t size)
{
    return system_getcwd(buf, size);
}

CRYSTAX_LOCAL
int driver_t::getdents(unsigned int fd, struct dirent *entry, unsigned int count)
{
    return system_getdents(fd, entry, count);
}

CRYSTAX_LOCAL
int driver_t::ioctl(int fd, int request, va_list &vl)
{
    return system_ioctl_v(fd, request, vl);
}

CRYSTAX_LOCAL
int driver_t::lchown(const char *path, uid_t uid, gid_t gid)
{
    return system_lchown(path, uid, gid);
}

CRYSTAX_LOCAL
int driver_t::link(const char *src, const char *dst)
{
    return system_link(src, dst);
}

CRYSTAX_LOCAL
off_t driver_t::lseek(int fd, off_t offset, int whence)
{
    return system_lseek(fd, offset, whence);
}

CRYSTAX_LOCAL
loff_t driver_t::lseek64(int fd, loff_t offset, int whence)
{
    return system_lseek64(fd, offset, whence);
}

CRYSTAX_LOCAL
int driver_t::lstat(const char *path, struct stat *st)
{
    return system_lstat(path, st);
}

CRYSTAX_LOCAL
int driver_t::mkdir(const char *path, mode_t mode)
{
    return system_mkdir(path, mode);
}

CRYSTAX_LOCAL
int driver_t::open(const char *path, int oflag, va_list &vl)
{
    return system_open_v(path, oflag, vl);
}

CRYSTAX_LOCAL
DIR *driver_t::opendir(const char *dirpath)
{
    return system_opendir(dirpath);
}

CRYSTAX_LOCAL
ssize_t driver_t::pread(int fd, void *buf, size_t count, off_t offset)
{
    return system_pread(fd, buf, count, offset);
}

CRYSTAX_LOCAL
ssize_t driver_t::pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    return system_pwrite(fd, buf, count, offset);
}

CRYSTAX_LOCAL
ssize_t driver_t::read(int fd, void *buf, size_t count)
{
    return system_read(fd, buf, count);
}

CRYSTAX_LOCAL
int driver_t::readv(int fd, const struct iovec *iov, int count)
{
    return system_readv(fd, iov, count);
}

CRYSTAX_LOCAL
struct dirent *driver_t::readdir(DIR *dirp)
{
    return system_readdir(dirp);
}

CRYSTAX_LOCAL
int driver_t::readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    return system_readdir_r(dirp, entry, result);
}

CRYSTAX_LOCAL
int driver_t::readlink(const char *path, char *buf, size_t bufsize)
{
    return system_readlink(path, buf, bufsize);
}

CRYSTAX_LOCAL
int driver_t::remove(const char *path)
{
    return system_remove(path);
}

CRYSTAX_LOCAL
int driver_t::rename(const char *oldpath, const char *newpath)
{
    return system_rename(oldpath, newpath);
}

CRYSTAX_LOCAL
void driver_t::rewinddir(DIR *dirp)
{
    return system_rewinddir(dirp);
}

CRYSTAX_LOCAL
int driver_t::rmdir(const char *path)
{
    return system_rmdir(path);
}

CRYSTAX_LOCAL
int driver_t::scandir(const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **))
{
    return system_scandir(dir, namelist, filter, compar);
}

CRYSTAX_LOCAL
void driver_t::seekdir(DIR *dirp, long offset)
{
    system_seekdir(dirp, offset);
}

CRYSTAX_LOCAL
int driver_t::select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv)
{
    return system_select(maxfd, rfd, wfd, efd, tv);
}

CRYSTAX_LOCAL
int driver_t::stat(const char *path, struct stat *st)
{
    return system_stat(path, st);
}

CRYSTAX_LOCAL
int driver_t::symlink(const char *src, const char *dst)
{
    return system_symlink(src, dst);
}

CRYSTAX_LOCAL
long driver_t::telldir(DIR *dirp)
{
    return system_telldir(dirp);
}

CRYSTAX_LOCAL
int driver_t::unlink(const char *path)
{
    return system_unlink(path);
}

CRYSTAX_LOCAL
ssize_t driver_t::write(int fd, const void *buf, size_t count)
{
    return system_write(fd, buf, count);
}

CRYSTAX_LOCAL
int driver_t::writev(int fd, const struct iovec *iov, int count)
{
    return system_writev(fd, iov, count);
}

} // namespace system
} // namespace fileio

CRYSTAX_LOCAL
int system_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    MODULE_INIT;
    return fileio::system::func_accept(sockfd, addr, addrlen);
}

CRYSTAX_LOCAL
int system_access(const char *path, int mode)
{
    MODULE_INIT;
    return fileio::system::func_access(path, mode);
}

CRYSTAX_LOCAL
int system_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    MODULE_INIT;
    return fileio::system::func_bind(sockfd, addr, addrlen);
}

CRYSTAX_LOCAL
int system_chdir(const char *path)
{
    MODULE_INIT;
    return fileio::system::func_chdir(path);
}

CRYSTAX_LOCAL
int system_chown(const char *path, uid_t uid, gid_t gid)
{
    MODULE_INIT;
    return fileio::system::func_chown(path, uid, gid);
}

CRYSTAX_LOCAL
int system_chroot(const char *path)
{
    MODULE_INIT;
    return fileio::system::func_chroot(path);
}

CRYSTAX_LOCAL
int system_close(int fd)
{
    MODULE_INIT;
    return fileio::system::func_close(fd);
}

CRYSTAX_LOCAL
int system_closedir(DIR *dirp)
{
    MODULE_INIT;
    return fileio::system::func_closedir(dirp);
}

CRYSTAX_LOCAL
int system_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    MODULE_INIT;
    return fileio::system::func_connect(sockfd, addr, addrlen);
}

CRYSTAX_LOCAL
int system_dirfd(DIR *dirp)
{
    MODULE_INIT;
    return fileio::system::func_dirfd(dirp);
}

CRYSTAX_LOCAL
int system_dup(int fd)
{
    MODULE_INIT;
    return fileio::system::func_dup(fd);
}

CRYSTAX_LOCAL
int system_dup2(int fd, int fd2)
{
    MODULE_INIT;
    return fileio::system::func_dup2(fd, fd2);
}

CRYSTAX_LOCAL
int system_fchdir(int fd)
{
    MODULE_INIT;
    return fileio::system::func_fchdir(fd);
}

CRYSTAX_LOCAL
int system_fchown(int fd, uid_t uid, gid_t gid)
{
    MODULE_INIT;
    return fileio::system::func_fchown(fd, uid, gid);
}

CRYSTAX_LOCAL
int system_fcntl(int fd, int command, ...)
{
    MODULE_INIT;
    va_list vl;
    va_start(vl, command);
    int ret = system_fcntl_v(fd, command, vl);
    va_end(vl);
    return ret;
}

CRYSTAX_LOCAL
int system_fcntl_v(int fd, int command, va_list &vl)
{
    MODULE_INIT;
    void *arg = va_arg(vl, void *);
    return fileio::system::func_fcntl(fd, command, arg);
}

CRYSTAX_LOCAL
int system_fdatasync(int fd)
{
    MODULE_INIT;
    return fileio::system::func_fdatasync(fd);
}

CRYSTAX_LOCAL
DIR *system_fdopendir(int fd)
{
    MODULE_INIT;
    return fileio::system::func_fdopendir(fd);
}

CRYSTAX_LOCAL
int system_flock(int fd, int operation)
{
    MODULE_INIT;
    return fileio::system::func_flock(fd, operation);
}

CRYSTAX_LOCAL
int system_fstat(int fd, struct stat *st)
{
    MODULE_INIT;
    return fileio::system::func_fstat(fd, st);
}

CRYSTAX_LOCAL
int system_fsync(int fd)
{
    MODULE_INIT;
    return fileio::system::func_fsync(fd);
}

CRYSTAX_LOCAL
int system_ftruncate(int fd, off_t offset)
{
    MODULE_INIT;
    return fileio::system::func_ftruncate(fd, offset);
}

CRYSTAX_LOCAL
char *system_getcwd(char *buf, size_t size)
{
    MODULE_INIT;
    return fileio::system::func_getcwd(buf, size);
}

CRYSTAX_LOCAL
int system_getdents(unsigned int fd, struct dirent *entry, unsigned int count)
{
    MODULE_INIT;
    return fileio::system::func_getdents(fd, entry, count);
}

CRYSTAX_LOCAL
int system_getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    MODULE_INIT;
    if (!fileio::system::func_getpwnam_r)
    {
        errno = ENOENT;
        return -1;
    }
    return fileio::system::func_getpwnam_r(name, pwd, buf, buflen, result);
}

CRYSTAX_LOCAL
int system_getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result)
{
    MODULE_INIT;
    if (!fileio::system::func_getpwuid_r)
    {
        errno = ENOENT;
        return -1;
    }
    return fileio::system::func_getpwuid_r(uid, pwd, buf, buflen, result);
}

CRYSTAX_LOCAL
int system_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    MODULE_INIT;
    return fileio::system::func_getsockname(sockfd, addr, addrlen);
}

CRYSTAX_LOCAL
int system_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
{
    MODULE_INIT;
    return fileio::system::func_getsockopt(sockfd, level, optname, optval, optlen);
}

CRYSTAX_LOCAL
int system_ioctl(int fd, int request, ...)
{
    MODULE_INIT;
    va_list vl;
    va_start(vl, request);
    int ret = system_ioctl_v(fd, request, vl);
    va_end(vl);
    return ret;
}

CRYSTAX_LOCAL
int system_ioctl_v(int fd, int request, va_list &vl)
{
    MODULE_INIT;
    void *arg = va_arg(vl, void *);
    return fileio::system::func_ioctl(fd, request, arg);
}

CRYSTAX_LOCAL
int system_lchown(const char *path, uid_t uid, gid_t gid)
{
    MODULE_INIT;
    return fileio::system::func_lchown(path, uid, gid);
}

CRYSTAX_LOCAL
int system_link(const char *src, const char *dst)
{
    MODULE_INIT;
    return fileio::system::func_link(src, dst);
}

CRYSTAX_LOCAL
int system_listen(int sockfd, int backlog)
{
    MODULE_INIT;
    return fileio::system::func_listen(sockfd, backlog);
}

CRYSTAX_LOCAL
off_t system_lseek(int fd, off_t offset, int whence)
{
    MODULE_INIT;
    return fileio::system::func_lseek(fd, offset, whence);
}

CRYSTAX_LOCAL
loff_t system_lseek64(int fd, loff_t offset, int whence)
{
    MODULE_INIT;
    return fileio::system::func_lseek64(fd, offset, whence);
}

CRYSTAX_LOCAL
int system_lstat(const char *path, struct stat *st)
{
    MODULE_INIT;
    return fileio::system::func_lstat(path, st);
}

CRYSTAX_LOCAL
int system_mkdir(const char *path, mode_t mode)
{
    MODULE_INIT;
    return fileio::system::func_mkdir(path, mode);
}

CRYSTAX_LOCAL
int system_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    MODULE_INIT;
    return fileio::system::func_mmap(addr, length, prot, flags, fd, offset);
}

CRYSTAX_LOCAL
int system_mount(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data)
{
    MODULE_INIT;
    return fileio::system::func_mount(source, target, fstype, flags, data);
}

CRYSTAX_LOCAL
int system_open(const char *path, int oflag, ...)
{
    MODULE_INIT;
    va_list vl;
    va_start(vl, oflag);
    int ret = system_open_v(path, oflag, vl);
    va_end(vl);
    return ret;
}

CRYSTAX_LOCAL
int system_open_v(const char *path, int oflag, va_list &vl)
{
    MODULE_INIT;
    if (oflag & O_CREAT)
    {
        int mode = va_arg(vl, int);
        return fileio::system::func_open(path, oflag, mode);
    }

    return fileio::system::func_open(path, oflag);
}

CRYSTAX_LOCAL
DIR *system_opendir(const char *dirpath)
{
    MODULE_INIT;
    return fileio::system::func_opendir(dirpath);
}

CRYSTAX_LOCAL
int system_pipe(int pipefd[2])
{
    MODULE_INIT;
    return fileio::system::func_pipe(pipefd);
}

CRYSTAX_LOCAL
ssize_t system_pread(int fd, void *buf, size_t count, off_t offset)
{
    MODULE_INIT;
    return fileio::system::func_pread(fd, buf, count, offset);
}

CRYSTAX_LOCAL
ssize_t system_pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    MODULE_INIT;
    return fileio::system::func_pwrite(fd, buf, count, offset);
}

CRYSTAX_LOCAL
int system_pthread_create(pthread_t *pth, pthread_attr_t const *pattr, void * (*func)(void *), void *arg)
{
    MODULE_INIT;
    return fileio::system::func_pthread_create(pth, pattr, func, arg);
}

CRYSTAX_LOCAL
ssize_t system_read(int fd, void *buf, size_t count)
{
    MODULE_INIT;
    return fileio::system::func_read(fd, buf, count);
}

CRYSTAX_LOCAL
int system_readv(int fd, const struct iovec *iov, int count)
{
    MODULE_INIT;
    return fileio::system::func_readv(fd, iov, count);
}

CRYSTAX_LOCAL
struct dirent *system_readdir(DIR *dirp)
{
    MODULE_INIT;
    return fileio::system::func_readdir(dirp);
}

CRYSTAX_LOCAL
int system_readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    MODULE_INIT;
    return fileio::system::func_readdir_r(dirp, entry, result);
}

CRYSTAX_LOCAL
int system_readlink(const char *path, char *buf, size_t bufsize)
{
    MODULE_INIT;
    return fileio::system::func_readlink(path, buf, bufsize);
}

CRYSTAX_LOCAL
ssize_t system_recv(int sockfd, void *buf, size_t len, unsigned int flags)
{
    MODULE_INIT;
    return fileio::system::func_recv(sockfd, buf, len, flags);
}

CRYSTAX_LOCAL
ssize_t system_recvfrom(int sockfd, void *buf, size_t len, unsigned int flags,
    const struct sockaddr *addr, socklen_t *addrlen)
{
    MODULE_INIT;
    return fileio::system::func_recvfrom(sockfd, buf, len, flags, addr, addrlen);
}

CRYSTAX_LOCAL
int system_recvmsg(int sockfd, struct msghdr *msg, unsigned int flags)
{
    MODULE_INIT;
    return fileio::system::func_recvmsg(sockfd, msg, flags);
}

CRYSTAX_LOCAL
int system_remove(const char *path)
{
    MODULE_INIT;
    return fileio::system::func_remove(path);
}

CRYSTAX_LOCAL
int system_rename(const char *oldpath, const char *newpath)
{
    MODULE_INIT;
    return fileio::system::func_rename(oldpath, newpath);
}

CRYSTAX_LOCAL
void system_rewinddir(DIR *dirp)
{
    MODULE_INIT;
    return fileio::system::func_rewinddir(dirp);
}

CRYSTAX_LOCAL
int system_rmdir(const char *path)
{
    MODULE_INIT;
    return fileio::system::func_rmdir(path);
}

CRYSTAX_LOCAL
int system_scandir(const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **))
{
    MODULE_INIT;
    return fileio::system::func_scandir(dir, namelist, filter, compar);
}

CRYSTAX_LOCAL
void system_seekdir(DIR *dirp, long offset)
{
    MODULE_INIT;
    if (fileio::system::func_seekdir)
        fileio::system::func_seekdir(dirp, offset);
}

CRYSTAX_LOCAL
int system_select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv)
{
    MODULE_INIT;
    return fileio::system::func_select(maxfd, rfd, wfd, efd, tv);
}

CRYSTAX_LOCAL
ssize_t system_send(int sockfd, const void *buf, size_t len, unsigned int flags)
{
    MODULE_INIT;
    return fileio::system::func_send(sockfd, buf, len, flags);
}

CRYSTAX_LOCAL
int system_sendmsg(int sockfd, const struct msghdr *msg, unsigned int flags)
{
    MODULE_INIT;
    return fileio::system::func_sendmsg(sockfd, msg, flags);
}

CRYSTAX_LOCAL
ssize_t system_sendto(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *addr, socklen_t addrlen)
{
    MODULE_INIT;
    return fileio::system::func_sendto(sockfd, buf, len, flags, addr, addrlen);
}

CRYSTAX_LOCAL
int system_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    MODULE_INIT;
    return fileio::system::func_setsockopt(sockfd, level, optname, optval, optlen);
}

CRYSTAX_LOCAL
int system_shutdown(int sockfd, int how)
{
    MODULE_INIT;
    return fileio::system::func_shutdown(sockfd, how);
}

CRYSTAX_LOCAL
int system_socket(int domain, int type, int protocol)
{
    MODULE_INIT;
    return fileio::system::func_socket(domain, type, protocol);
}

CRYSTAX_LOCAL
int system_stat(const char *path, struct stat *st)
{
    MODULE_INIT;
    return fileio::system::func_stat(path, st);
}

CRYSTAX_LOCAL
int system_symlink(const char *src, const char *dst)
{
    MODULE_INIT;
    return fileio::system::func_symlink(src, dst);
}

CRYSTAX_LOCAL
long system_telldir(DIR *dirp)
{
    MODULE_INIT;
    if (!fileio::system::func_telldir)
    {
        errno = ENOTSUP;
        return -1;
    }

    return fileio::system::func_telldir(dirp);
}

CRYSTAX_LOCAL
int system_umount(const char *target)
{
    MODULE_INIT;
    return fileio::system::func_umount(target);
}

CRYSTAX_LOCAL
int system_umount2(const char *target, int flags)
{
    MODULE_INIT;
    return fileio::system::func_umount2(target, flags);
}

CRYSTAX_LOCAL
int system_unlink(const char *path)
{
    MODULE_INIT;
    return fileio::system::func_unlink(path);
}

CRYSTAX_LOCAL
ssize_t system_write(int fd, const void *buf, size_t count)
{
    MODULE_INIT;
    return fileio::system::func_write(fd, buf, count);
}

CRYSTAX_LOCAL
int system_writev(int fd, const struct iovec *iov, int count)
{
    MODULE_INIT;
    return fileio::system::func_writev(fd, iov, count);
}

} // namespace crystax
