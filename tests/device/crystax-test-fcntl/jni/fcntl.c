#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if __APPLE__ || __gnu_linux__
#define FNAME "/tmp/crystax-ndk-fcntl-test.txt"
#elif __ANDROID__
#define FNAME "/data/local/tmp/ndk-fcntl.txt"
#else
#error "Unknown platform"
#endif

static void test_F_GETFD_and_F_SETFD(int fd)
{
    int rc;

    rc = fcntl(fd, F_GETFD);
    assert(rc == 0);

    rc = fcntl(fd, F_SETFD, FD_CLOEXEC);
    assert(rc == 0);

    rc = fcntl(fd, F_GETFD);
    assert(rc == FD_CLOEXEC);

    rc = fcntl(fd, F_SETFD, 0);
    assert(rc == 0);

    rc = fcntl(fd, F_GETFD);
    assert(rc == 0);
}

static void test_F_DUPFD(int fd)
{
    int rc = fcntl(fd, F_DUPFD, fd + 1);
    assert(rc >= fd + 1);

    int rc2 = fcntl(fd, F_DUPFD, fd + 1);
    assert(rc2 >= fd + 2);

    close(rc);
    close(rc2);
}

static void test_F_DUPFD_CLOEXEC(int fd)
{
    int rc = fcntl(fd, F_DUPFD_CLOEXEC, fd + 1);
    assert(rc >= fd + 1);

    int rc2 = fcntl(rc, F_GETFD);
    assert(rc2 == FD_CLOEXEC);

    close(rc);
}

static void test_F_DUP2FD(int fd)
{
#if __APPLE__ || __gnu_linux__
    // There is no F_DUP2FD flag on OS X and GNU/Linux systems
    (void)fd;
#else
    int rc = fcntl(fd, F_DUP2FD, fd + 1);
    assert(rc == fd + 1);

    close(rc);
#endif
}

static void test_F_DUP2FD_CLOEXEC(int fd)
{
#if __APPLE__ || __gnu_linux__
    // There is no F_DUP2FD_CLOEXEC flag on OS X and GNU/Linux systems
    (void)fd;
#else
    int rc = fcntl(fd, F_DUP2FD_CLOEXEC, fd + 1);
    assert(rc == fd + 1);

    int rc2 = fcntl(rc, F_GETFD);
    assert(rc2 == FD_CLOEXEC);

    close(rc);
#endif
}

static void test_F_GETFL_and_F_SETFL(int fd)
{
    int oflags = fcntl(fd, F_GETFL);

#define TEST_FLAG(f)                              \
    assert(fcntl(fd, F_SETFL, oflags & ~f) == 0); \
    assert((fcntl(fd, F_GETFL) & f) == 0);        \
    assert(fcntl(fd, F_SETFL, oflags | f) == 0);  \
    assert((fcntl(fd, F_GETFL) & f) != 0);

    TEST_FLAG(O_NONBLOCK);
    TEST_FLAG(O_APPEND);

#undef TEST_FLAG

    assert(fcntl(fd, F_SETFL, oflags) == 0);
}

static void test_F_GETLK_and_F_SETLK(int fd)
{
    struct flock fl = {
        .l_start = 0,
        .l_len = 0,
        .l_whence = SEEK_SET,
        .l_pid = getpid(),
        .l_type = F_RDLCK,
    };

    assert(fcntl(fd, F_GETLK, &fl) == 0);
    assert(fl.l_start == 0);
    assert(fl.l_len == 0);
    assert(fl.l_whence == SEEK_SET);
    assert(fl.l_pid == getpid());
    assert(fl.l_type == F_UNLCK);

    fl.l_start = 0;
    fl.l_len = 1;
    fl.l_whence = SEEK_SET;
    fl.l_pid = getpid();
    fl.l_type = F_RDLCK;
    assert(fcntl(fd, F_SETLK, &fl) == 0);

    assert(fcntl(fd, F_GETLK, &fl) == 0);
    assert(fl.l_start == 0);
    assert(fl.l_len == 1);
    assert(fl.l_whence == SEEK_SET);
    assert(fl.l_pid == getpid());
    assert(fl.l_type == F_UNLCK);

    fl.l_start = 0;
    fl.l_len = 1;
    fl.l_whence = SEEK_SET;
    fl.l_pid = getpid();
    fl.l_type = F_WRLCK;
    assert(fcntl(fd, F_SETLK, &fl) == 0);

    assert(fcntl(fd, F_GETLK, &fl) == 0);
    assert(fl.l_start == 0);
    assert(fl.l_len == 1);
    assert(fl.l_whence == SEEK_SET);
    assert(fl.l_pid == getpid());
    assert(fl.l_type == F_UNLCK);

    fl.l_start = 0;
    fl.l_len = 1;
    fl.l_whence = SEEK_SET;
    fl.l_pid = getpid();
    fl.l_type = F_WRLCK;
    assert(fcntl(fd, F_SETLKW, &fl) == 0);

    assert(fcntl(fd, F_GETLK, &fl) == 0);
    assert(fl.l_start == 0);
    assert(fl.l_len == 1);
    assert(fl.l_whence == SEEK_SET);
    assert(fl.l_pid == getpid());
    assert(fl.l_type == F_UNLCK);
}

int main()
{
    int fd;

    unlink(FNAME);

    fd = open(FNAME, O_RDWR | O_CREAT, S_IRWXU);
    assert(fd == 3);
    assert(write(fd, "1", 1) == 1);

    test_F_GETFD_and_F_SETFD(fd);
    test_F_GETFL_and_F_SETFL(fd);
    test_F_DUPFD(fd);
    test_F_DUPFD_CLOEXEC(fd);
    test_F_DUP2FD(fd);
    test_F_DUP2FD_CLOEXEC(fd);
    test_F_GETLK_and_F_SETLK(fd);

    close(fd);
    unlink(FNAME);

    return 0;
}
