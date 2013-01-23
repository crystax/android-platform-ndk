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

#include "assets/driver.hpp"

#define METADATA_V1 1

#define JCHECK \
    do \
    { \
        if (!::crystax::jni::jexcheck()) \
        { \
            ERR("java exception occured"); \
            ::abort(); \
        } \
    } while(0)

namespace crystax
{
namespace fileio
{
namespace assets
{

using jni::jhclass;
using jni::jhobject;
using jni::jhstring;
using jni::jhbyteArray;
using jni::jhobjectArray;

using jni::jnienv;
using jni::find_class;
using jni::get_method_id;
using jni::get_field_id;

static int const ACCESS_STREAMING = 2;

#if 1
#define dump_stat(...) do {} while(0)
#else
static void dump_stat(struct stat const &st)
{
    DBG("st.st_dev: %llx", (long long)st.st_dev);
    DBG("st.st_ino: %lx", (long)st.st_ino);
    DBG("st.st_mode: %x", (int)st.st_mode);
    DBG("st.st_nlink: %x", (int)st.st_nlink);
    DBG("st.st_uid: %lx", (unsigned long)st.st_uid);
    DBG("st.st_gid: %lx", (unsigned long)st.st_gid);
    DBG("st.st_rdev: %llx", (long long)st.st_rdev);
    DBG("st.st_size: %lld", (long long)st.st_size);
    DBG("st.st_blksize: %lu", (unsigned long)st.st_blksize);
    DBG("st.st_blocks: %llu", (unsigned long long)st.st_blocks);
    DBG("st.st_atime: %lu", (unsigned long)st.st_atime);
    DBG("st.st_atime_nsec: %lu", (unsigned long)st.st_atime_nsec);
    DBG("st.st_mtime: %lu", (unsigned long)st.st_mtime);
    DBG("st.st_mtime_nsec: %lu", (unsigned long)st.st_mtime_nsec);
    DBG("st.st_ctime: %lu", (unsigned long)st.st_ctime);
    DBG("st.st_ctime_nsec: %lu", (unsigned long)st.st_ctime_nsec);
    DBG("st.st_ino: %llx", (unsigned long long)st.st_ino);
}
#endif

static void fill_stat(JNIEnv *env, jhobject const &objContext, fileio::driver_t *d, struct stat *st)
{
    jmethodID midContextGetPackageName = get_method_id(
        env, objContext, "getPackageName", "()Ljava/lang/String;");
    JCHECK;
    jhstring objPackageName((jstring)env->CallObjectMethod(
        objContext.get(), midContextGetPackageName));
    JCHECK;

    jmethodID midContextGetPackageManager = get_method_id(
        env, objContext, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    JCHECK;
    jhobject objPackageManager(env->CallObjectMethod(
        objContext.get(), midContextGetPackageManager));
    JCHECK;

    jmethodID midPackageManagerGetApplicationInfo = get_method_id(
        env, objPackageManager, "getApplicationInfo",
        "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    JCHECK;
    jhobject objApplicationInfo(env->CallObjectMethod(
        objPackageManager.get(), midPackageManagerGetApplicationInfo,
        objPackageName.get(), 0));
    JCHECK;

    jfieldID fidAppInfoDataDir = get_field_id(
        env, objApplicationInfo, "dataDir", "Ljava/lang/String;");
    JCHECK;
    jhstring objDataDir((jstring)env->GetObjectField(objApplicationInfo.get(), fidAppInfoDataDir));
    JCHECK;

    abspath_t datadir(jcast<const char *>(objDataDir));
    JCHECK;
    DBG("datadir=%s", datadir.c_str());

    char *s = (char *)::malloc(datadir.length() + 5);
    ::strcpy(s, datadir.c_str());
    ::strcat(s, "/lib");
    abspath_t libdir(s);
    ::free((void*)s);
    DBG("libdir=%s", libdir.c_str());

    if (d->stat(libdir.c_str(), st) < 0)
    {
        ERR("can't get stat for %s", libdir.c_str());
        ::abort();
    }

    st->st_mode = S_IFREG|S_IRWXU;
    st->st_nlink = 1;
    st->st_uid = ::getuid();
    st->st_gid = ::getgid();

    DBG("source stat: mode=%d, uid=%d, gid=%d, atime=%lu, mtime=%lu, ctime=%lu",
        (int)st->st_mode, (int)st->st_uid, (int)st->st_gid,
        (unsigned long)st->st_atime, (unsigned long)st->st_mtime, (unsigned long)st->st_ctime);
}

CRYSTAX_LOCAL
void driver_t::init_jni(JNIEnv *env, jhobject const &objContext)
{
    jmethodID midContextGetAssets = get_method_id(
        env, objContext, "getAssets", "()Landroid/content/res/AssetManager;");
    JCHECK;
    jhobject objAM = jni::call_method<jhobject>(env, objContext, midContextGetAssets);
    JCHECK;
    objAssetManager = env->NewGlobalRef(objAM.get());

    midAmOpen = get_method_id(env, objAM,
        "open", "(Ljava/lang/String;I)Ljava/io/InputStream;");
    JCHECK;

    midAmList = get_method_id(env, objAM,
        "list", "(Ljava/lang/String;)[Ljava/lang/String;");
    JCHECK;

    jhclass clsInputStream = find_class(env, "java/io/InputStream");
    JCHECK;

    midIsClose = get_method_id(env, clsInputStream, "close", "()V");
    JCHECK;

    midIsAvail = get_method_id(env, clsInputStream, "available", "()I");
    JCHECK;

    midIsRead = get_method_id(env, clsInputStream, "read", "([B)I");
    JCHECK;

    midIsSkip = get_method_id(env, clsInputStream, "skip", "(J)J");
    JCHECK;

    midIsMark = get_method_id(env, clsInputStream, "mark", "(I)V");
    JCHECK;

    midIsReset = get_method_id(env, clsInputStream, "reset", "()V");
    JCHECK;
}

CRYSTAX_LOCAL
bool driver_t::read_metadata_entry(int fd, abspath_t *abspath, bool *removed)
{
    uint8_t version;
    ssize_t n = underlying()->read(fd, &version, sizeof(version));
    if (n != sizeof(version))
    {
        ERR("can't read version field");
        return false;
    }
    if (version != METADATA_V1)
    {
        ERR("Unknown metadata version: %d", version);
        return false;
    }

    uint32_t length;
    n = underlying()->read(fd, &length, sizeof(length));
    if (n != sizeof(length))
    {
        ERR("can't read length field");
        return false;
    }

    scope_c_ptr_t<char> path((char*)::malloc(length + 1));
    n = underlying()->read(fd, path.get(), length);
    if ((size_t)n != length)
    {
        ERR("can't read path field");
        return false;
    }
    path[length] = '\0';

    uint8_t flag;
    n = underlying()->read(fd, &flag, sizeof(flag));
    if (n != sizeof(flag))
    {
        ERR("can't read flag field");
        return false;
    }

    abspath->reset(path.get());
    *removed = (bool)flag;

    return true;
}

CRYSTAX_LOCAL
bool driver_t::write_metadata_entry(int fd, abspath_t const &abspath, bool removed)
{
    uint8_t version = METADATA_V1;

    ssize_t n = underlying()->write(fd, &version, sizeof(version));
    if (n != sizeof(version))
    {
        ERR("can't write version field");
        return false;
    }

    uint32_t length = abspath.length();
    n = underlying()->write(fd, &length, sizeof(length));
    if (n != sizeof(length))
    {
        ERR("can't write length field");
        return false;
    }

    n = underlying()->write(fd, abspath.c_str(), abspath.length());
    if ((size_t)n != abspath.length())
    {
        ERR("can't write path field");
        return false;
    }

    uint8_t flag = removed ? 1 : 0;
    n = underlying()->write(fd, &flag, sizeof(flag));
    if (n != sizeof(flag))
    {
        ERR("can't write flag field");
        return false;
    }

    return true;
}

CRYSTAX_LOCAL
void driver_t::load_metadata()
{
    scope_lock_t guard(metadata_mutex);

    struct stat st;
    abspath_t mpath(root().c_str());
    mpath += ".metadata";
    if (underlying()->stat(mpath.c_str(), &st) < 0)
        return;

    int fd = underlying()->open(mpath.c_str(), O_RDONLY);
    if (fd < 0)
        return;

    for (;;)
    {
        abspath_t abspath;
        bool removed;
        if (!read_metadata_entry(fd, &abspath, &removed))
            break;

        DBG("add metadata entry: %s", abspath.c_str());
        metadata.push_back(new metadata_entry_t(abspath.c_str(), removed));
    }
    underlying()->close(fd);
}

CRYSTAX_LOCAL
void driver_t::save_metadata()
{
    scope_lock_t guard(metadata_mutex);

    abspath_t mpath(root().c_str());
    mpath += ".metadata";
    int fd = underlying()->open(mpath.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    if (fd < 0)
    {
        ERR("can't open metadata for save");
        ::abort();
    }

    for (metadata_entry_t *entry = metadata.head(); entry; entry = entry->next)
    {
        if (!write_metadata_entry(fd, entry->path, entry->removed))
        {
            ERR("can't write metadata entry: %s", entry->path.c_str());
            ::abort();
        }
    }

    underlying()->close(fd);
}

CRYSTAX_LOCAL
bool driver_t::metadata_comparator(metadata_entry_t const &e, const char *path)
{
    abspath_t abspath(path);
    return e.path == abspath;
}

CRYSTAX_LOCAL
driver_t::driver_t(const char *root, jobject obj, fileio::driver_t *d)
    :fileio::driver_t(root, d)
{
    pthread_mutexattr_t attr;
    if (::pthread_mutexattr_init(&attr) != 0)
        ::abort();
    if (::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        ::abort();

    if (::pthread_mutex_init(&fd_table_mutex, &attr) != 0)
        ::abort();
    init_fd();

    JNIEnv *env = jnienv();

    jhobject objContext(obj);
    fill_stat(env, objContext, underlying(), &sst);
    init_jni(env, objContext);

    if (::pthread_mutex_init(&metadata_mutex, &attr) != 0)
        ::abort();
    //load_metadata();

    if (::pthread_mutexattr_destroy(&attr) != 0)
        ::abort();
}

CRYSTAX_LOCAL
driver_t::~driver_t()
{
    jnienv()->DeleteGlobalRef(objAssetManager);

    if (::pthread_mutex_destroy(&fd_table_mutex) != 0)
        ::abort();
    if (::pthread_mutex_destroy(&metadata_mutex) != 0)
        ::abort();
}

CRYSTAX_LOCAL
void driver_t::init_fd()
{
    scope_lock_t lock(fd_table_mutex);
    for (size_t fd = 0; fd != sizeof(fd_table)/sizeof(fd_table[0]); ++fd)
    {
        fd_entry_t &e = fd_table[fd];
        e.obj = NULL;
        e.pos = 0;
        e.size = 0;
        e.extfd = -1;
        e.path.reset();
    }
}

CRYSTAX_LOCAL
int driver_t::alloc_fd(jobject obj, size_t size, abspath_t const &abspath)
{
    DBG("obj=%p", obj);
    if (obj == NULL)
        return -1;

    JNIEnv *env = jnienv();
    scope_lock_t lock(fd_table_mutex);
    for (size_t fd = 0; fd != sizeof(fd_table)/sizeof(fd_table[0]); ++fd)
    {
        fd_entry_t &e = fd_table[fd];
        if (e.obj != NULL || e.extfd != -1)
            continue;

        DBG("NewGlobalRef for obj %p", obj);
        e.obj = env->NewGlobalRef(obj);
        e.pos = 0;
        e.size = size;
        e.extfd = -1;
        e.path.reset(::strdup(abspath.c_str()));
        return fd;
    }

    return -1;
}

CRYSTAX_LOCAL
int driver_t::alloc_fd(int extfd, abspath_t const &abspath)
{
    DBG("extfd=%d", extfd);
    if (extfd < 0)
        return -1;

    scope_lock_t lock(fd_table_mutex);
    for (size_t fd = 0; fd != sizeof(fd_table)/sizeof(fd_table[0]); ++fd)
    {
        fd_entry_t &e = fd_table[fd];
        if (e.obj != NULL || e.extfd != -1)
            continue;

        e.obj = NULL;
        e.pos = 0;
        e.size = 0;
        e.extfd = extfd;
        e.path.reset(::strdup(abspath.c_str()));
        return fd;
    }

    return -1;
}

CRYSTAX_LOCAL
void driver_t::free_fd(int fd)
{
    DBG("fd=%d", fd);
    if (fd < 0 || (size_t)fd >= sizeof(fd_table)/sizeof(fd_table[0]))
        return;

    JNIEnv *env = jnienv();

    scope_lock_t lock(fd_table_mutex);

    fd_entry_t &e = fd_table[fd];
    if (e.obj)
        env->DeleteGlobalRef(e.obj);
    e.obj = NULL;
    e.pos = 0;
    e.size = 0;
    e.extfd = -1;
    e.path.reset();
}

CRYSTAX_LOCAL
bool driver_t::resolve(int fd, jobject *obj, size_t *pos, size_t *size, int *extfd, abspath_t *abspath)
{
    DBG("fd=%d", fd);
    if (fd < 0 || (size_t)fd >= sizeof(fd_table)/sizeof(fd_table[0]))
        return false;

    scope_lock_t lock(fd_table_mutex);

    fd_entry_t &e = fd_table[fd];
    if (obj) *obj = e.obj;
    if (pos) *pos = e.pos;
    if (size) *size = e.size;
    if (extfd) *extfd = e.extfd;
    if (abspath) abspath->reset(::strdup(e.path.c_str()));
    return true;
}

CRYSTAX_LOCAL
bool driver_t::update(int fd, size_t pos)
{
    DBG("fd=%d", fd);
    if (fd < 0 || (size_t)fd >= sizeof(fd_table)/sizeof(fd_table[0]))
        return false;

    scope_lock_t lock(fd_table_mutex);

    fd_entry_t &e = fd_table[fd];
    e.pos = pos;
    return true;
}

CRYSTAX_LOCAL
bool driver_t::check_subpath(abspath_t const &abspath)
{
    //DBG("abspath=%s", abspath.c_str());
    if (!abspath.subpath(root()))
    {
        //ERR("path %s is not subpath of %s", abspath.c_str(), root().c_str());
        errno = EINVAL;
        return false;
    }
    //DBG("path %s is subpath of %s", abspath.c_str(), root().c_str());
    return true;
}

CRYSTAX_LOCAL
int driver_t::chown(const char * /* path */, uid_t /* uid */, gid_t /* gid */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::close(int fd)
{
    DBG("fd=%d", fd);

    JNIEnv *env = jnienv();

    jobject obj;
    int extfd;
    if (!resolve(fd, &obj, NULL, NULL, &extfd, NULL))
    {
        ERR("can't resolve fd=%d", fd);
        errno = EINVAL;
        return -1;
    }

    DBG("use obj=%p", obj);
    jhobject objInputStream(obj ? env->NewLocalRef(obj) : 0);
    free_fd(fd);

    if (objInputStream)
    {
        jni::call_method<void>(env, objInputStream, midIsClose);
        env->ExceptionClear();
    }

    if (extfd >= 0)
    {
        DBG("extfd=%d", extfd);
        underlying()->close(extfd);
    }

    DBG("return 0");
    return 0;
}

CRYSTAX_LOCAL
int driver_t::closedir(DIR *dirp)
{
    TRACE;
    return underlying()->closedir(dirp);
}

CRYSTAX_LOCAL
bool driver_t::copy_from_assets(abspath_t const &abspath, path_t const &rpath)
{
    DBG("abspath=%s, rpath=%s", abspath.c_str(), rpath.c_str());
    abspath_t dir(abspath.dirname());
    if (mkdir_p(dir, S_IRWXU) != 0)
    {
        TRACE;
        return false;
    }

    TRACE;
    JNIEnv *env = jnienv();

    jhobject objInputStream = jni::call_method<jhobject>(env,
        objAssetManager, midAmOpen, jcast<jhstring>(rpath), ACCESS_STREAMING);
    if (env->ExceptionCheck())
    {
        TRACE;
        env->ExceptionClear();
        errno = ENOENT;
        return false;
    }

    TRACE;
    int fd = underlying()->open(abspath.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    DBG("fd=%d", fd);
    if (fd == -1)
    {
        jni::call_method<void>(env, objInputStream, midIsClose);
        env->ExceptionClear();
        return false;
    }

    char buf[4096];
    jhbyteArray objArray(env->NewByteArray(sizeof(buf)));
    for (;;)
    {
        jint n = jni::call_method<jint>(env, objInputStream, midIsReset, objArray);
        DBG("n=%d", n);
        bool success = !env->ExceptionCheck();
        env->ExceptionClear();
        if (!success)
        {
            TRACE;
            jni::call_method<void>(env, objInputStream, midIsClose);
            env->ExceptionClear();
            underlying()->close(fd);
            errno = EFAULT;
            return false;
        }

        TRACE;
        if (n < 0)
        {
            TRACE;
            break;
        }

        TRACE;
        env->GetByteArrayRegion(objArray.get(), 0, n, (jbyte*)buf);
        if (underlying()->write(fd, buf, n) < 0)
        {
            TRACE;
            int save_errno = errno;
            jni::call_method<void>(env, objInputStream, midIsClose);
            env->ExceptionClear();
            underlying()->close(fd);
            errno = save_errno;
            return false;
        }
    }

    TRACE;
    jni::call_method<void>(env, objInputStream, midIsClose);
    env->ExceptionClear();
    underlying()->close(fd);

    return true;
}

CRYSTAX_LOCAL
int driver_t::dirfd(DIR * /* dirp */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::dup(int /* fd */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::dup2(int /* fd */, int /* fd2 */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::fchown(int fd, uid_t uid, gid_t gid)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::fcntl(int /* fd */, int /* command */, va_list &/* vl */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::fdatasync(int /* fd */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
DIR *driver_t::fdopendir(int /* fd */)
{
    NOT_IMPLEMENTED_NULL;
}

CRYSTAX_LOCAL
int driver_t::flock(int /* fd */, int /* operation */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::fstat(int fd, struct stat *st)
{
    abspath_t abspath;
    if (!resolve(fd, NULL, NULL, NULL, NULL, &abspath))
    {
        ERR("wrong fd passed");
        errno = EINVAL;
        return -1;
    }

    return stat(abspath.c_str(), st);
}

CRYSTAX_LOCAL
int driver_t::fsync(int /* fd */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::ftruncate(int /* fd */, off_t /* offset */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::getdents(unsigned int /* fd */, struct dirent * /* entry */, unsigned int /* count */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::ioctl(int fd, int request, va_list &vl)
{
    DBG("fd=%d, request=%d", fd, request);

    jobject obj;
    int extfd;
    if (!resolve(fd, &obj, NULL, NULL, &extfd, NULL))
    {
        errno = EINVAL;
        return -1;
    }

    if (extfd != -1)
    {
        DBG("use extfd=%d", extfd);
        return underlying()->ioctl(extfd, request, vl);
    }

    DBG("use obj=%p", obj);

    JNIEnv *env = jnienv();

    switch (request)
    {
    case FIONREAD:
        {
            int *avail = va_arg(vl, int *);
            if (avail == NULL)
            {
                errno = EINVAL;
                return -1;
            }

            *avail = jni::call_method<jint>(env, obj, midIsAvail);
            if (env->ExceptionCheck())
            {
                env->ExceptionClear();
                errno = EFAULT;
                return -1;
            }

            return 0;
        }
        break;
    default:
        NOT_IMPLEMENTED;
    }
}

CRYSTAX_LOCAL
int driver_t::lchown(const char * /* path */, uid_t /* uid */, gid_t /* gid */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::link(const char * /* src */, const char * /* dst */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
off_t driver_t::lseek(int fd, off_t offset, int whence)
{
    return lseek64(fd, offset, whence);
}

CRYSTAX_LOCAL
loff_t driver_t::lseek64(int fd, loff_t offset, int whence)
{
    DBG("fd=%d, offset=%ld, whence=%d", fd, (long)offset, whence);

    jobject obj;
    size_t pos;
    size_t size;
    int extfd;
    if (!resolve(fd, &obj, &pos, &size, &extfd, NULL))
    {
        errno = EINVAL;
        return -1;
    }

    if (extfd != -1)
    {
        DBG("use extfd=%d", extfd);
        return underlying()->lseek64(extfd, offset, whence);
    }

    DBG("use obj=%p", obj);

    JNIEnv *env = jnienv();

    switch (whence)
    {
    case SEEK_SET:
        if (offset < 0)
            offset = 0;
        if (offset > size)
            offset = size;
        pos = offset;
        DBG("fd=%d: SEEK_SET: pos=%lld", fd, (long long)pos);
        break;
    case SEEK_CUR:
        if (pos + offset < 0)
            offset = -pos;
        if (pos + offset > size)
            offset = size - pos;
        pos += offset;
        DBG("fd=%d: SEEK_CUR: pos=%lld", fd, (long long)pos);
        break;
    case SEEK_END:
        if (offset > 0)
            offset = 0;
        if (-offset > size)
            offset = -size;
        pos = size + offset;
        DBG("fd=%d: SEEK_END: pos=%lld", fd, (long long)pos);
        break;
    default:
        DBG("fd=%d: unknown whence value: %d", fd, whence);
    }

    jni::call_method<void>(env, obj, midIsReset);
    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }
    jni::call_method<jlong>(env, obj, midIsSkip, (jlong)pos);
    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }

    update(fd, pos);

    return pos;
}

CRYSTAX_LOCAL
int driver_t::lstat(const char * /* path */, struct stat * /* st */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::mkdir(const char *path, mode_t mode)
{
    return underlying()->mkdir(path, mode);
}

CRYSTAX_LOCAL
int driver_t::mkdir_p(abspath_t const &abspath, mode_t mode)
{
    DBG("abspath=%s", abspath.c_str());
    if (!abspath || abspath.length() > PATH_MAX)
    {
        errno = EINVAL;
        return -1;
    }

    char buf[PATH_MAX + 1];

    for (const char *s = abspath.c_str(), *p = s + 1; s; p = s + 1)
    {
        s = ::strchr(p, '/');
        size_t len = s ? s - abspath.c_str() : abspath.length();
        ::strncpy(buf, abspath.c_str(), len);
        buf[len] = '\0';
        int e = mkdir(buf, mode);
        if (e == -1 && errno != EEXIST)
            return -1;
    }

    return 0;
}

CRYSTAX_LOCAL
int driver_t::open(const char *path, int oflag, va_list &vl)
{
    DBG("path=%s, oflag=%d", path, oflag);

    abspath_t abspath(path);
    if (!check_subpath(abspath))
        return -1;

    // {
    //     DBG("check metadata...");
    //     scope_lock_t guard(metadata_mutex);
    //     metadata_entry_t *entry = metadata.find(metadata_comparator, abspath.c_str());
    //     if (entry)
    //     {
    //         DBG("metadata entry found");
    //         if (entry->removed)
    //         {
    //             if ((oflag & O_CREAT) == 0)
    //             {
    //                 errno = ENOENT;
    //                 return -1;
    //             }

    //             entry->removed = false;
    //         }
    //     }
    // }

    path_t rpath(abspath.relpath(root()));
    DBG("rpath=%s", rpath.c_str());

    bool ul = false;
    struct stat ulst;
    if (oflag & (O_WRONLY|O_RDWR))
    {
        if (!copy_from_assets(abspath, rpath) && errno != ENOENT)
            return -1;
        ul = true;
    }
    else if (stat_ul(abspath, rpath, &ulst) == 0)
        ul = true;

    if (ul)
    {
        DBG("pass to underlying driver");
        int extfd = underlying()->open(path, oflag, vl);
        DBG("extfd=%d", extfd);
        int fd = alloc_fd(extfd, abspath);
        if (fd < 0)
        {
            DBG("can't alloc fd");
            underlying()->close(extfd);
            errno = EMFILE;
            return -1;
        }
        DBG("return fd=%d", fd);
        return fd;
    }

    JNIEnv *env = jnienv();

    jhobject objInputStream = jni::call_method<jhobject>(env, objAssetManager, midAmOpen,
        jcast<jhstring>(rpath), ACCESS_STREAMING);
    if (env->ExceptionCheck())
    {
        ERR("can't open %s", rpath.c_str());
        env->ExceptionClear();
        errno = ENOENT;
        return -1;
    }
    DBG("objInputStream=%p", objInputStream.get());

    jni::call_method<void>(env, objInputStream, midIsMark, (jint)INT_MAX);
    if (env->ExceptionCheck())
    {
        ERR("can't set mark on stream");
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }
    size_t size = (size_t)jni::call_method<jlong>(env, objInputStream, midIsSkip, (jlong)INT_MAX);
    if (env->ExceptionCheck())
    {
        ERR("can't skip stream to the end");
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }
    DBG("size=%u", (unsigned)size);
    jni::call_method<void>(env, objInputStream, midIsReset);
    if (env->ExceptionCheck())
    {
        ERR("can't reset stream");
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }

    int fd = alloc_fd(objInputStream.get(), size, abspath);
    if (fd < 0)
    {
        ERR("can't alloc fd");
        jni::call_method<void>(env, objInputStream, midIsClose);
        env->ExceptionClear();
        errno = EMFILE;
        return -1;
    }

    DBG("return fd=%d", fd);
    return fd;
}

CRYSTAX_LOCAL
DIR *driver_t::opendir(const char *dirpath)
{
    DBG("dirpath=%s", dirpath);
    return underlying()->opendir(dirpath);
}

CRYSTAX_LOCAL
ssize_t driver_t::pread(int /* fd */, void * /* buf */, size_t /* count */, off_t /* offset */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
ssize_t driver_t::pwrite(int /* fd */, const void * /* buf */, size_t /* count */, off_t /* offset */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
ssize_t driver_t::read(int fd, void *buf, size_t count)
{
    DBG("fd=%d, count=%u", fd, (unsigned)count);

    if (count == 0)
    {
        DBG("request 0 bytes");
        return 0;
    }

    jobject obj;
    size_t pos;
    int extfd;
    if (!resolve(fd, &obj, &pos, NULL, &extfd, NULL))
    {
        ERR("wrong fd passed");
        errno = EINVAL;
        return -1;
    }

    if (extfd != -1)
    {
        DBG("use extfd=%d", extfd);
        return underlying()->read(extfd, buf, count);
    }

    DBG("use obj=%p", obj);
    JNIEnv *env = jnienv();

    jhbyteArray objArray(env->NewByteArray(count));
    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        ERR("can't allocate %u bytes", (unsigned)count);
        errno = ENOMEM;
        return -1;
    }

    jint n = jni::call_method<jint>(env, obj, midIsRead, objArray);
    if (env->ExceptionCheck())
    {
        ERR("java read failed");
        env->ExceptionClear();
        errno = EFAULT;
        return -1;
    }
    if (n < 0)
    {
        DBG("eof reached");
        return 0;
    }

    if ((size_t)n > count)
    {
        ERR("java read return more than requested");
        errno = EFAULT;
        return -1;
    }

    DBG("java read return %d", (int)n);
    pos += n;
    update(fd, pos);

    env->GetByteArrayRegion(objArray.get(), 0, n, (jbyte*)buf);

    DBG("return %d bytes", (unsigned)n);
    return n;
}

CRYSTAX_LOCAL
int driver_t::readv(int /* fd */, const struct iovec * /* iov */, int /* count */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
struct dirent *driver_t::readdir(DIR *dirp)
{
    TRACE;
    return underlying()->readdir(dirp);
}

CRYSTAX_LOCAL
int driver_t::readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
    TRACE;
    return underlying()->readdir_r(dirp, entry, result);
}

CRYSTAX_LOCAL
int driver_t::readlink(const char * /* path */, char * /* buf */, size_t /* bufsize */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::remove(const char *path)
{
    DBG("path=%s", path);
    return underlying()->remove(path);
}

CRYSTAX_LOCAL
int driver_t::rename(const char *oldpath, const char *newpath)
{
    DBG("oldpath=%s, newpath=%s", oldpath, newpath);

    fileio::driver_t *d = find_driver(oldpath);
    if (!d)
        return -1;

    if (d != underlying())
    {
        ERR("cross device rename not supported");
        errno = EXDEV;
        return -1;
    }

    // scope_lock_t guard(metadata_mutex);
    // metadata_entry_t *entry = metadata.find(metadata_comparator, oldpath);
    // if (entry && entry->removed)
    // {
    //     errno = ENOENT;
    //     return -1;
    // }

    if (underlying()->rename(oldpath, newpath) < 0)
        return -1;

    // if (!entry)
    // {
    //     entry = new metadata_entry_t(oldpath, true);
    //     metadata.push_back(entry);
    // }

    // entry->removed = true;

    return 0;
}

CRYSTAX_LOCAL
void driver_t::rewinddir(DIR * /* dirp */)
{
    NOT_IMPLEMENTED_BASE;
}

CRYSTAX_LOCAL
int driver_t::rmdir(const char *path)
{
    DBG("path=%s", path);
    return underlying()->rmdir(path);
}

CRYSTAX_LOCAL
int driver_t::scandir(const char * /* dir */, struct dirent *** /* namelist */, int (* /* filter */)(const struct dirent *),
    int (* /* compar */)(const struct dirent **, const struct dirent **))
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
void driver_t::seekdir(DIR * /* dirp */, long /* offset */)
{
    NOT_IMPLEMENTED_BASE;
}

CRYSTAX_LOCAL
int driver_t::select(int /* maxfd */, fd_set * /* rfd */, fd_set * /* wfd */, fd_set * /* efd */, struct timeval * /* tv */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::stat(const char *path, struct stat *st)
{
    DBG("path=%s", path);

    abspath_t abspath(path);
    if (!check_subpath(abspath))
        return -1;

    path_t rpath(abspath.relpath(root()));
    DBG("rpath=%s", rpath.c_str());

    if (stat_ul(abspath, rpath, st) == 0)
        return 0;

    return stat_as(rpath, st);

    // struct stat ulst, asst;
    // DBG("ask underlying driver");
    // int ule = underlying()->stat(abspath.c_str(), &ulst);
    // DBG("ask assets driver");
    // int ase = stat_as(rpath, &asst);
    // if (ase == 0 && (ule != 0 || ulst.st_mtime < asst.st_mtime))
    // {
    //     DBG("use asset's stat");
    //     underlying()->unlink(abspath.c_str());
    //     ::memcpy(st, &asst, sizeof(asst));
    //     return 0;
    // }
    // if (ule == 0 && (ase != 0 || ulst.st_mtime >= asst.st_mtime))
    // {
    //     DBG("use underlying stat");
    //     ::memcpy(st, &ulst, sizeof(ulst));
    //     return 0;
    // }
    // 
    // DBG("no such entry");
    // errno = ENOENT;
    // return -1;
}

CRYSTAX_LOCAL
int driver_t::stat_as(path_t const &rpath, struct stat *st)
{
    JNIEnv *env = jnienv();

    DBG("is it file?");
    jhstring objPath = jcast<jhstring>(rpath);
    jhobject objInputStream = jni::call_method<jhobject>(env, objAssetManager, midAmOpen,
        objPath, ACCESS_STREAMING);
    env->ExceptionClear();
    if (objInputStream)
    {
        DBG("it is file");

        jlong skipped = jni::call_method<jlong>(env, objInputStream, midIsSkip, (jlong)INT_MAX);
        env->ExceptionClear();
        jni::call_method<void>(env, objInputStream, midIsClose);
        env->ExceptionClear();

        ::memcpy(st, &sst, sizeof(struct stat));
        st->st_mode = S_IFREG|S_IRWXU;
        st->st_size = (size_t)skipped;
        dump_stat(*st);
        return 0;
    }

    DBG("is it directory?");
    jhobjectArray objArray = jni::call_method<jhobjectArray>(env, objAssetManager, midAmList, objPath);
    env->ExceptionClear();
    if (objArray && env->GetArrayLength(objArray.get()) > 0)
    {
        DBG("it is directory");
        ::memcpy(st, &sst, sizeof(struct stat));
        st->st_mode = S_IFDIR|S_IRWXU;
        dump_stat(*st);
        return 0;
    }

    DBG("no such entry");
    errno = ENOENT;
    return -1;
}

CRYSTAX_LOCAL
int driver_t::stat_ul(abspath_t const &abspath, path_t const &rpath, struct stat *st)
{
    int ret = underlying()->stat(abspath.c_str(), st);
    if (ret == 0)
    {
        DBG("entry found");
        dump_stat(*st);
    }
    return ret;
    // struct stat ulst, asst;
    // bool ulexists = underlying()->stat(abspath.c_str(), &ulst);
    // bool asexists = stat_as(rpath, &asst);
    // if (asexists && (!ulexists || ulst.st_mtime < asst.st_mtime))
    // {
    //     underlying()->unlink(abspath.c_str());
    //     ulexists = false;
    // }
    // 
    // if (!ulexists)
    // {
    //     errno = ENOENT;
    //     return -1;
    // }
    // 
    // ::memcpy(st, &ulst, sizeof(ulst));
    // return 0;
}

CRYSTAX_LOCAL
int driver_t::symlink(const char * /* src */, const char * /* dst */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
long driver_t::telldir(DIR * /* dirp */)
{
    NOT_IMPLEMENTED;
}

CRYSTAX_LOCAL
int driver_t::unlink(const char *path)
{
    // scope_lock_t guard(metadata_mutex);

    // metadata_entry_t *entry = metadata.find(metadata_comparator, path);
    // if (!entry)
    // {
    //     entry = new metadata_entry_t(path, false);
    //     if (!metadata.push_back(entry))
    //     {
    //         errno = EFAULT;
    //         return -1;
    //     }
    // }
    // entry->removed = true;
    // save_metadata();

    underlying()->unlink(path);

    return 0;
}

CRYSTAX_LOCAL
ssize_t driver_t::write(int fd, const void *buf, size_t count)
{
    DBG("fd=%d, count=%u", fd, (unsigned)count);

    int extfd;
    if (!resolve(fd, NULL, NULL, NULL, &extfd, NULL))
    {
        ERR("wrong fd passed");
        errno = EINVAL;
        return -1;
    }

    return underlying()->write(extfd, buf, count);
}

CRYSTAX_LOCAL
int driver_t::writev(int /* fd */, const struct iovec * /* iov */, int /* count */)
{
    NOT_IMPLEMENTED;
}

} // namespace assets
} // namespace fileio
} // namespace crystax
