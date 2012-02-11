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

#ifndef _CRYSTAX_PRIVATE_H_99544c48e9174f659a97671e7f64c763
#define _CRYSTAX_PRIVATE_H_99544c48e9174f659a97671e7f64c763

#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <pwd.h>
#include <assert.h>
#include <android/log.h>

#include <crystax.h>

#ifdef __cplusplus
#include <crystax/common.hpp>
#include <crystax/memory.hpp>
#include "crystax/jutils.hpp"
#endif

#ifndef CRYSTAX_DEBUG
#define CRYSTAX_DEBUG 0
#endif

#define CRYSTAX_SINK_STDOUT 0
#define CRYSTAX_SINK_STDERR 1
#define CRYSTAX_SINK_LOGCAT 2

#ifndef CRYSTAX_DEBUG_SINK
#define CRYSTAX_DEBUG_SINK CRYSTAX_SINK_LOGCAT
#endif

#define __SHORT_FILE_MAXLEN__ 25
#define __SHORT_FILE__ \
    (strlen(__FILE__) < __SHORT_FILE_MAXLEN__ ? \
        __FILE__ : \
        __FILE__ + (strlen(__FILE__) - __SHORT_FILE_MAXLEN__) \
    )

#if CRYSTAX_DEBUG
#   define CRYSTAX_DEBUG_FORMAT "(%08x) ...%s:%-5d: %-15s: " 
#   if CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_STDOUT
#       define CRYSTAX_LOG(level, fmt, ...) \
            printf("CRYSTAX_" #level ": " \
                CRYSTAX_DEBUG_FORMAT fmt "\n", \
                (unsigned)::pthread_self(), __SHORT_FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#   elif CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_STDERR
#       define CRYSTAX_LOG(level, fmt, ...) \
            fprintf(stderr, "CRYSTAX_" #level ": " \
                CRYSTAX_DEBUG_FORMAT fmt "\n", \
                (unsigned)::pthread_self(), __SHORT_FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#   elif CRYSTAX_DEBUG_SINK == CRYSTAX_SINK_LOGCAT
#       define CRYSTAX_LOG(level, fmt, ...) \
            __android_log_print(ANDROID_LOG_DEBUG, "CRYSTAX_" #level, \
                CRYSTAX_DEBUG_FORMAT fmt, \
                (unsigned)::pthread_self(), __SHORT_FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#   endif
#else
#   define CRYSTAX_LOG(...) do {} while(0)
#endif

#define DBG(fmt, ...)  CRYSTAX_LOG(DBUG, fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) CRYSTAX_LOG(INFO, fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) CRYSTAX_LOG(WARN, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)  CRYSTAX_LOG(ERRO, fmt, ##__VA_ARGS__)

#define TRACE DBG("***")

#ifdef __cplusplus
#   define CRYSTAX_GLOBAL extern "C" __attribute__ ((visibility ("default")))
#else
#   define CRYSTAX_GLOBAL __attribute__ ((visibility ("default")))
#endif
#define CRYSTAX_LOCAL  __attribute__ ((visibility ("hidden")))

#define FD_TABLE_SIZE 1024

#define __dead2
#define EFTYPE EFAULT
#define EX_OSERR -1

#define reallocf realloc

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) MIN(b, a)
#endif

#define OFF_MAX LLONG_MAX
#define OFF_MIN LLONG_MIN

#define FLOCKFILE(fp) do {} while(0)
#define FUNLOCKFILE(fp) do {} while(0)

#define DEFFILEMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

#define __SIGN 0x8000

#define powerof2(x) ((((x)-1)&(x))==0)

typedef struct {
    char *data;
    size_t size;
} __crystax_locale_data_t;

typedef struct
{
    char *encoding;
    struct
    {
        int alias;
        char *aliasto;
        __crystax_locale_data_t data;
    } data[_LC_LAST];
} __crystax_locale_whole_data_t;

#ifdef __i386__
typedef enum {
  FP_PS=0,  /* 24 bit (single-precision) */
  FP_PRS,   /* reserved */
  FP_PD,    /* 53 bit (double-precision) */
  FP_PE   /* 64 bit (extended-precision) */
} fp_prec_t;

extern fp_prec_t fpsetprec(fp_prec_t p);
#endif

#ifdef __cplusplus
extern "C" {
#endif

int __fflush(FILE *fp);
int fdatasync(int fd);

const char *__crystax_getprogname();
mbstate_t *__crystax_get_mbstate(FILE *fp);

int __crystax_locale_init();
__crystax_locale_whole_data_t *__crystax_locale_lookup_whole_data(const char *encoding);
__crystax_locale_data_t * __crystax_locale_get_part_data(const char *encoding, const char *data);
__crystax_locale_data_t * __crystax_locale_get_data(int type, const char *encoding);

int __crystax_fileio_init();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace crystax
{
namespace fileio
{

bool init();

} // namespace fileio

int system_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int system_access(const char *path, int mode);
int system_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int system_chdir(const char *path);
int system_chown(const char *path, uid_t uid, gid_t gid);
int system_chroot(const char *path);
int system_close(int fd);
int system_closedir(DIR *dirp);
int system_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int system_dirfd(DIR *dirp);
int system_dup(int fd);
int system_dup2(int fd, int fd2);
int system_fchdir(int fd);
int system_fchown(int fd, uid_t uid, gid_t gid);
int system_fcntl(int fd, int command, ...);
int system_fcntl_v(int fd, int command, va_list &vl);
int system_fdatasync(int fd);
DIR *system_fdopendir(int fd);
int system_flock(int fd, int operation);
int system_fstat(int fd, struct stat *st);
int system_fsync(int fd);
int system_ftruncate(int fd, off_t offset);
char *system_getcwd(char *buf, size_t size);
int system_getdents(unsigned int fd, struct dirent *entry, unsigned int count);
int system_getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);
int system_getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);
int system_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int system_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int system_ioctl(int fd, int request, ...);
int system_ioctl_v(int fd, int request, va_list &vl);
int system_lchown(const char *path, uid_t uid, gid_t gid);
int system_link(const char *src, const char *dst);
int system_listen(int sockfd, int backlog);
off_t system_lseek(int fd, off_t offset, int whence);
loff_t system_lseek64(int fd, loff_t offset, int whence);
int system_lstat(const char *path, struct stat *st);
int system_mkdir(const char *path, mode_t mode);
int system_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int system_mount(const char *source, const char *target, const char *fstype,
    unsigned long flags, const void *data);
int system_open(const char *path, int oflag, ...);
int system_open_v(const char *path, int oflag, va_list &vl);
DIR *system_opendir(const char *dirpath);
int system_pipe(int pipefd[2]);
ssize_t system_pread(int fd, void *buf, size_t count, off_t offset);
ssize_t system_pwrite(int fd, const void *buf, size_t count, off_t offset);
int system_pthread_create(pthread_t *pth, pthread_attr_t const *pattr, void * (*func)(void *), void *arg);
ssize_t system_read(int fd, void *buf, size_t count);
int system_readv(int fd, const struct iovec *iov, int count);
struct dirent *system_readdir(DIR *dirp);
int system_readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
int system_readlink(const char *path, char *buf, size_t bufsize);
ssize_t system_recv(int sockfd, void *buf, size_t len, unsigned int flags);
ssize_t system_recvfrom(int sockfd, void *buf, size_t len, unsigned int flags,
    const struct sockaddr *addr, socklen_t *addrlen);
int system_recvmsg(int sockfd, struct msghdr *msg, unsigned int flags);
int system_remove(const char *path);
int system_rename(const char *oldpath, const char *newpath);
void system_rewinddir(DIR *dirp);
int system_rmdir(const char *path);
int system_scandir(const char *dir, struct dirent ***namelist, int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **));
void system_seekdir(DIR *dirp, long offset);
int system_select(int maxfd, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *tv);
ssize_t system_send(int sockfd, const void *buf, size_t len, unsigned int flags);
int system_sendmsg(int sockfd, const struct msghdr *msg, unsigned int flags);
ssize_t system_sendto(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *addr, socklen_t addrlen);
int system_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int system_shutdown(int sockfd, int how);
int system_socket(int domain, int type, int protocol);
int system_stat(const char *path, struct stat *st);
int system_symlink(const char *src, const char *dst);
long system_telldir(DIR *dirp);
int system_umount2(const char *target, int flags);
int system_unlink(const char *path);
ssize_t system_write(int fd, const void *buf, size_t count);
int system_writev(int fd, const struct iovec *iov, int count);

} // namespace crystax
#endif

#endif /* _CRYSTAX_PRIVATE_H_99544c48e9174f659a97671e7f64c763 */
