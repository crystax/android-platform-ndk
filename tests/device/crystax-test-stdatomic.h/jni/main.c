#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9))
#define HAVE_STD_ATOMIC 1
#else
#define HAVE_STD_ATOMIC 0
#endif

#if __gnu_linux__ && __clang__
/* There is bug in clang on GNU/Linux, when it uses gcc's stdatomic.h instead of own one
 * and this cause compiler error.
 */
#undef HAVE_STD_ATOMIC
#define HAVE_STD_ATOMIC 0
#endif

#if HAVE_STD_ATOMIC

#include <stdatomic.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 200
#define MAX_THREAD_COUNTER 100000

atomic_int value = ATOMIC_VAR_INIT(0);
int nvalue = 0;

void *worker(void *arg)
{
    (void)arg;

    int i;

    for (i = 0; i < MAX_THREAD_COUNTER; ++i) {
        atomic_fetch_add(&value, 1);
        ++nvalue;
    }
    for (i = 0; i < MAX_THREAD_COUNTER/2; ++i) {
        atomic_fetch_sub(&value, 15);
        nvalue -= 15;
    }
    for (i = 0; i < MAX_THREAD_COUNTER/2; ++i) {
        atomic_fetch_add(&value, 10);
        nvalue += 10;
    }
    for (i = 0; i < MAX_THREAD_COUNTER/2; ++i) {
        atomic_fetch_add(&value, 5);
        nvalue += 5;
    }

    return NULL;
}

void test()
{
    int i;
    int v;
    pthread_t ths[MAX_THREADS];

    printf("=== Starting %d threads...\n", MAX_THREADS);
    for (i = 0; i < MAX_THREADS; ++i)
        assert(pthread_create(&ths[i], NULL, &worker, NULL) == 0);

    printf("=== Joining %d threads...\n", MAX_THREADS);
    for (i = 0; i < MAX_THREADS; ++i)
        assert(pthread_join(ths[i], NULL) == 0);

    v = atomic_load(&value);
    printf("=== Atomic counter: %d\n", v);
    printf("=== Non-atomic counter: %d\n", nvalue);

    assert(v == MAX_THREADS * MAX_THREAD_COUNTER);
}

#endif /* HAVE_STD_ATOMIC */

int main()
{
#if HAVE_STD_ATOMIC
    test();
#endif
    return 0;
}
