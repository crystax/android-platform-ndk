#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>

#define DO_TEST(name, param)                           \
    printf("test " #name "(" #param "): BEGIN\n");     \
    fflush(stdout);                                    \
    test_ ## name (param);                             \
    printf("test " #name "(" #param "): OK\n");        \
    fflush(stdout)

struct thread_fun_args {
    int sec;
    pthread_mutex_t *mutex;
};

#if !defined(__ANDROID__)
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
#endif

pthread_mutex_t fast_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rec_mutex      = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
pthread_mutex_t errcheck_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;


void test_lock_unlock(pthread_mutex_t *mutex);
void test_expired(pthread_mutex_t *mutex);
void test_with_thread(pthread_mutex_t *mutex);

void lock_mutex_on_thread(pthread_mutex_t *mutex, int seconds);
void *thread_fun(void *);
void get_time(struct timespec *ts);
void nano_sleep(int sec);

int main()
{
    DO_TEST(lock_unlock, &fast_mutex);
    DO_TEST(lock_unlock, &rec_mutex);
    DO_TEST(lock_unlock, &errcheck_mutex);
    DO_TEST(expired,     &fast_mutex);
    DO_TEST(expired,     &rec_mutex);
    DO_TEST(expired,     &errcheck_mutex);
    DO_TEST(with_thread, &fast_mutex);
    DO_TEST(with_thread, &rec_mutex);
    DO_TEST(with_thread, &errcheck_mutex);

    return 0;
}

#define FAIL_IF(c, fmt, ...) do { if (c) fail("FATAL: %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); } while (0)

void fail(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    abort();
}

const char *mutex_type(pthread_mutex_t *mutex)
{
    if (mutex == &fast_mutex)
        return "normal";
    else if (mutex == &rec_mutex)
        return "recursive";
    else if (mutex == &errcheck_mutex)
        return "errorcheck";

    fprintf(stderr, "FATAL: unknown mutex passed to 'mutex_type' function\n");
    abort();
}

void test_lock_unlock(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    get_time(&timeout);
    timeout.tv_sec += 3;

    rc = pthread_mutex_timedlock(mutex, &timeout);
    FAIL_IF(rc != 0, "pthread_mutex_timedlock failed to lock %s mutex: %s", mutex_type(mutex), strerror(rc));

    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (mutex == &rec_mutex)
        FAIL_IF(rc != 0, "pthread_mutex_timedlock failed to lock second time recursive mutex: %s", strerror(rc));
    else
        FAIL_IF(rc == 0, "pthread_mutex_timedlock successfully locked second time non-recursive mutex, but it shouldn't");

    rc = pthread_mutex_unlock(mutex);
    FAIL_IF(rc != 0, "pthread_mutex_unlock failed to unlock %s mutex: %s", mutex_type(mutex), strerror(rc));
    rc = pthread_mutex_unlock(mutex);
    if (mutex == &rec_mutex || mutex == &fast_mutex)
        FAIL_IF(rc != 0, "pthread_mutex_unlock failed to unlock %s mutex second time: %s", mutex_type(mutex), strerror(rc));
    else
        FAIL_IF(rc == 0, "pthread_mutex_unlock successfully unlocked %s mutex second time, but it shouldn't", mutex_type(mutex));
}


void test_expired(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    get_time(&timeout);
    timeout.tv_sec -= 3;

    /* Under no circumstance shall the function fail with a timeout if the mutex can be locked immediately.
     * The validity of the abs_timeout parameter need not be checked if the mutex can be locked immediately.
     */
    rc = pthread_mutex_timedlock(mutex, &timeout);
    FAIL_IF(rc != 0, "pthread_mutex_timedlock failed to lock %s mutex with expired timeout: %s", mutex_type(mutex), strerror(rc));

    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (mutex == &rec_mutex)
        FAIL_IF(rc != 0, "pthread_mutex_timedlock failed to lock recursive mutex second time with expired timeout: %s", strerror(rc));
    else
        FAIL_IF(rc == 0, "pthread_mutex_timedlock successfully locked non-recursive mutex second time with expired timeout, but it shouldn't");

    rc = pthread_mutex_unlock(mutex);
    FAIL_IF(rc != 0, "pthread_mutex_unlock failed to unlock %s mutex: %s", mutex_type(mutex), strerror(rc));
    rc = pthread_mutex_unlock(mutex);
    if (mutex == &rec_mutex || mutex == &fast_mutex)
        FAIL_IF(rc != 0, "pthread_mutex_unlock failed to unlock %s mutex second time: %s", mutex_type(mutex), strerror(rc));
    else
        FAIL_IF(rc == 0, "pthread_mutex_unlock successfully unlocked %s mutex second time, but it shouldn't", mutex_type(mutex));
}


void test_with_thread(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    /* lock must succeed */
    lock_mutex_on_thread(mutex, 2);
    nano_sleep(1);
    get_time(&timeout);
    timeout.tv_sec += 3;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    FAIL_IF(rc != 0, "pthread_mutex_timedlock failed: %s", strerror(rc));
    rc = pthread_mutex_unlock(mutex);
    FAIL_IF(rc != 0, "pthread_mutex_unlock failed: %s", strerror(rc));

    /* lock must timeout */
    lock_mutex_on_thread(mutex, 600);
    nano_sleep(10);
    get_time(&timeout);
    timeout.tv_sec += 2;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    FAIL_IF(rc != ETIMEDOUT, "pthread_mutex_timedlock failed: expected timeout, got %d (%s)", rc, strerror(rc));
}


void lock_mutex_on_thread(pthread_mutex_t *mutex, int seconds)
{
    int rc;
    pthread_t tid;
    pthread_attr_t attr;
    struct thread_fun_args *args;

    rc = pthread_attr_init(&attr);
    FAIL_IF(rc != 0, "pthread_attr_init failed: %s", strerror(rc));

    args = malloc(sizeof (struct thread_fun_args));
    FAIL_IF(!args, "no memory for thread args");
    args->sec = seconds;
    args->mutex = mutex;

    rc = pthread_create(&tid, &attr, thread_fun, args);
    FAIL_IF(rc != 0, "pthread_create failed: %s", strerror(rc));

    rc = pthread_detach(tid);
    FAIL_IF(rc != 0, "pthread_detach failed: %s", strerror(rc));
}


void *thread_fun(void *v)
{
    int rc;
    struct thread_fun_args *args = (struct thread_fun_args *) v;

    rc = pthread_mutex_lock(args->mutex);
    if (rc != 0) {
        printf("on thread: pthread_mutex_lock failed: %d, %s\n", rc, strerror(rc));
        exit(1);
    }

    nano_sleep(args->sec);

    rc = pthread_mutex_unlock(args->mutex);
    if (rc != 0) {
        printf("on thread: pthread_mutex_unlock failed: %d, %s\n", rc, strerror(rc));
        exit(1);
    }

    return 0;
}


void get_time(struct timespec *ts)
{
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        fprintf(stderr, "FATAL: clock_gettime failed: %s\n", strerror(errno));
        abort();
    }
}


void nano_sleep(int sec)
{
    struct timespec ts;
    struct timespec rem = { 0, 0 };

    ts.tv_sec = sec;
    ts.tv_nsec = 0;

    if (nanosleep(&ts, &rem) != 0) {
        fprintf(stderr, "FATAL: nanosleep failed: %s\n", strerror(errno));
        abort();
    }
}
