#include "common.h"

#if GCC_ATLEAST(4, 6)

#if !defined(_GLIBCXX_HAS_GTHREADS)
#error _GLIBCXX_HAS_GTHREADS undefined!
#endif

#if !defined(_GLIBCXX_USE_C99_STDINT_TR1)
#error _GLIBCXX_USE_C99_STDINT_TR1 undefined!
#endif

#if !defined(_GLIBCXX_USE_NANOSLEEP)
#error _GLIBCXX_USE_NANOSLEEP undefined!
#endif

#if !defined(_POSIX_TIMEOUTS)
#error _POSIX_TIMEOUTS undefined!
#endif

#include <iostream>
#include <utility>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

void f1(int n)
{
    for(int i=0; i<5; ++i) {
        std::cout << "Thread " << n << " executing\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void f2(int& n)
{
    for(int i=0; i<5; ++i) {
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
    std::cout << "Final value of n is " << n << '\n';

    return 0;
}


#else


int test_thread()
{
    return -1;
}


#endif  // GCC_ATLEAST(4, 7)
