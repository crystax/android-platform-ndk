#include "common.h"

#if defined(__GLIBCXX__)

#if !defined(_GLIBCXX_HAS_GTHREADS)
#error _GLIBCXX_HAS_GTHREADS undefined!
#endif

#if !defined(_GLIBCXX_USE_C99_STDINT_TR1)
#error _GLIBCXX_USE_C99_STDINT_TR1 undefined!
#endif

#if !defined(_GLIBCXX_USE_NANOSLEEP)
#error _GLIBCXX_USE_NANOSLEEP undefined!
#endif

#endif // __GLIBCXX__

#if defined(__ANDROID__) && !defined(_POSIX_TIMEOUTS)
#error _POSIX_TIMEOUTS undefined!
#endif

#include <iostream>
#include <utility>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

#define NUM 5

void f1(int n)
{
    for(int i=0; i<NUM; ++i) {
        std::cout << "Thread " << n << " executing\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void f2(int& n)
{
    for(int i=0; i<NUM; ++i) {
        std::cout << "Thread 2 executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int test_thread()
{
    int n = 0;
    std::thread t1(f1, n+1);
    std::thread t3(f2, std::ref(n));
    std::thread t4(std::move(t3));
    t1.join();
    t4.join();

    if (n == NUM)
        std::cout << "Final value of n is " << n << std::endl;
    else {
        std::cout << "error: unexpected value of n: " << n << std::endl;
        return 1;
    }

    return 0;
}
