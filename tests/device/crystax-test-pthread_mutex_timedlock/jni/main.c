#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>

#define DO_TEST(name, param)                           \
    printf("test %s - begin\n", #name);                \
    fflush(stdout);                                    \
    if (test_ ## name (param) == 0) {                  \
        printf("test " #name "(" #param "): OK\n");    \
    } else  {                                          \
        printf("FAILED test " #name "(" #param ")\n"); \
        return 1;                                      \
    }

struct thread_fun_args {
    int sec;
    pthread_mutex_t *mutex;
};

/* Linux has no static mutex initializers, except PTHREAD_MUTEX_INITIALIZER */
#ifdef __linux__
#define INIT(x)
#else
#define INIT(x) = x
#endif

pthread_mutex_t fast_mutex     INIT(PTHREAD_MUTEX_INITIALIZER);
pthread_mutex_t rec_mutex      INIT(PTHREAD_RECURSIVE_MUTEX_INITIALIZER);
pthread_mutex_t errcheck_mutex INIT(PTHREAD_ERRORCHECK_MUTEX_INITIALIZER);


int test_lock_unlock(pthread_mutex_t *mutex);
int test_einval(pthread_mutex_t *mutex);
int test_expired(pthread_mutex_t *mutex);
int test_with_thread(pthread_mutex_t *mutex);

int lock_mutex_on_thread(pthread_mutex_t *mutex, int seconds);
void *thread_fun(void *);
int get_time(struct timespec *ts);
void nano_sleep(int sec);

#ifdef __linux__
void checkrc(int rc)
{
    if (rc == 0) return;
    fprintf(stderr, "ERROR: %s\n", strerror(rc));
    fflush(stderr);
    abort();
}
#endif

int main()
{
    printf("begin\n");
    fflush(stdout);

#ifdef __linux__
    pthread_mutexattr_t attr;

    checkrc(pthread_mutexattr_init(&attr));
    checkrc(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
    checkrc(pthread_mutex_init(&fast_mutex, &attr));
    checkrc(pthread_mutexattr_destroy(&attr));

    checkrc(pthread_mutexattr_init(&attr));
    checkrc(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    checkrc(pthread_mutex_init(&rec_mutex, &attr));
    checkrc(pthread_mutexattr_destroy(&attr));

    checkrc(pthread_mutexattr_init(&attr));
    checkrc(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
    checkrc(pthread_mutex_init(&errcheck_mutex, &attr));
    checkrc(pthread_mutexattr_destroy(&attr));
#endif

#ifndef __linux__
    DO_TEST(einval,      NULL);
#endif
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


int test_einval(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif

    /* check NULL timeout */
    rc = pthread_mutex_timedlock(mutex, NULL);

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    if (rc != EINVAL) {
        printf("pthread_mutex_timedlock failed with NULL timeout\n");
        return 1;
    }

    /* check bad nsec values */
    if (get_time(&timeout) != 0)
        return 1;
    timeout.tv_nsec = 1000000000;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != EINVAL) {
        printf("pthread_mutex_timedlock failed with big nsec\n");
        return 1;
    }
    timeout.tv_nsec = -2;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != EINVAL) {
        printf("pthread_mutex_timedlock failed with negative nsec\n");
        return 1;
    }

    /* check with NULL mutex */
    timeout.tv_sec += 5;
    timeout.tv_nsec = 300;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
#endif

    rc = pthread_mutex_timedlock(NULL, &timeout);

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    if (rc != EINVAL) {
        printf("pthread_mutex_timedlock failed with NULL mutex\n");
        return 1;
    }

    return 0;
}


int test_lock_unlock(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    if (get_time(&timeout) != 0)
        return 1;
    timeout.tv_sec += 3;

    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != 0) {
        printf("pthread_mutex_timedlock failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    rc = pthread_mutex_unlock(mutex);
    if (rc != 0) {
        printf("pthread_mutex_unlock failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    return 0;
}


int test_expired(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    if (get_time(&timeout) != 0)
        return 1;
    timeout.tv_sec -= 3;

    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != ETIMEDOUT) {
        printf("pthread_mutex_timedlock failed with expired timeout: rc=%d (%s)\n", rc, strerror(rc));
        return 1;
    }

    return 0;
}


int test_with_thread(pthread_mutex_t *mutex)
{
    int rc;
    struct timespec timeout;

    /* lock must succeed */
    if (lock_mutex_on_thread(mutex, 2) != 0)
        return 1;
    nano_sleep(1);
    if (get_time(&timeout) != 0)
        return 1;
    timeout.tv_sec += 3;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != 0) {
        printf("pthread_mutex_timedlock failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }
    rc = pthread_mutex_unlock(mutex);
    if (rc != 0) {
        printf("pthread_mutex_unlock failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    /* lock must timeout */
    if (lock_mutex_on_thread(mutex, 5) != 0)
        return 1;
    nano_sleep(1);
    if (get_time(&timeout) != 0)
        return 1;
    timeout.tv_sec += 2;
    rc = pthread_mutex_timedlock(mutex, &timeout);
    if (rc != ETIMEDOUT) {
        printf("pthread_mutex_timedlock failed: expected timeout, got %d; %s\n", rc, strerror(rc));
        return 1;
    }

    return 0;
}


int lock_mutex_on_thread(pthread_mutex_t *mutex, int seconds)
{
    int rc;
    pthread_t tid;
    pthread_attr_t attr;
    struct thread_fun_args *args;

    rc = pthread_attr_init(&attr);
    if (rc != 0) {
        printf("pthread_attr_init failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    args = malloc(sizeof (struct thread_fun_args));
    if (args == NULL) {
        printf("no memory for thread args\n");
        return 1;
    }
    args->sec = seconds;
    args->mutex = mutex;

    rc = pthread_create(&tid, &attr, thread_fun, args);
    if (rc != 0) {
        printf("pthread_create failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    rc = pthread_detach(tid);
    if (rc != 0) {
        printf("pthread_detach failed: %d; %s\n", rc, strerror(rc));
        return 1;
    }

    return 0;
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


int get_time(struct timespec *ts)
{
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        printf("clock_gettime failed: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}


void nano_sleep(int sec)
{
    struct timespec ts;
    struct timespec rem = { 0, 0 };

    ts.tv_sec = sec;
    ts.tv_nsec = 0;

    if (nanosleep(&ts, &rem) != 0) {
        printf("nanosleep failed: %d, %s\n", errno, strerror(errno));
        exit(1);
    }
}
