#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>


#define DO_TEST(name, param)   if (test_ ## name (param) == 0) {                  \
                                   printf("test " #name "(" #param "): OK\n");    \
                               } else  {                                          \
                                   printf("FAILED test " #name "(" #param ")\n"); \
                                   return 1;                                      \
                               }

struct thread_fun_args {
    int sec;
    pthread_mutex_t *mutex;
};


static pthread_mutex_t fast_mutex     = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rec_mutex      = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
static pthread_mutex_t errcheck_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;


static int test_lock_unlock(pthread_mutex_t *mutex);
static int test_einval(pthread_mutex_t *mutex);
static int test_expired(pthread_mutex_t *mutex);
static int test_with_thread(pthread_mutex_t *mutex);

static int lock_mutex_on_thread(pthread_mutex_t *mutex, int seconds);
static void *thread_fun(void *);
static int get_time(struct timespec *ts);
static void nano_sleep(int sec);


int main()
{
    /* printf("sizeof(long)      = %d\n", sizeof(long)); */
    /* printf("sizeof(long long) = %d\n", sizeof(long long)); */

    DO_TEST(einval,      NULL);
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
        printf("pthread_mutex_timedlock failed with expired timeout\n");
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
