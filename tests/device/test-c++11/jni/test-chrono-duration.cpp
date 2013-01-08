#include "common.h"

#if GCC_ATLEAST(4, 7)

#include <sstream>
#include <ratio>
#include <chrono>


// very strange: min shoud be -9223372036854775808
// but gcc emits a warning about integer constant being too large...
// so we have: 9223372036854775808 == -9223372036854775808
// very strange...
#define SYS_CLOCK_MIN    9223372036854775808ULL
#define SYS_CLOCK_MAX    9223372036854775807


int test_chrono_duration_ctor()
{
    typedef std::chrono::duration<int> Seconds;
    typedef std::chrono::duration<int, std::milli> Milliseconds;
    typedef std::chrono::duration<int, std::ratio<60*60>> Hours;

    Hours        hoursperday(24);
    Seconds      secsperday(60*60*24);
    Milliseconds msperday(secsperday);
    Seconds      secsperhour(60*60);
    Hours        hourperhour(std::chrono::duration_cast<Hours>(secsperhour));
    Milliseconds msperhour(secsperhour);

    std::stringstream os;
    os << hoursperday.count() << std::endl
       << secsperday.count()  << std::endl
       << msperday.count()    << std::endl
       << secsperhour.count() << std::endl
       << hourperhour.count() << std::endl
       << msperhour.count()   << std::endl;

    std::string res =
        "24\n"
        "86400\n"
        "86400000\n"
        "3600\n"
        "1\n"
        "3600000\n";

    if (res != os.str()) {
        std::cout << "unexpected result: " << os.str() << std::endl;
        return 1;
    }

    return 0;
}


int test_chrono_duration_operators()
{
    std::chrono::duration<int> foo;
    std::chrono::duration<int> bar(1024);

    foo = bar;                 // 1024  1024
    foo = foo + bar;           // 2048  1024
    ++foo;                     // 2049  1024
    --bar;                     // 2049  1023
    foo *= 2;                  // 4098  1023
    foo /= 3;                  // 1366  1023
    bar += ( foo % bar );      // 1366  1366

    if ((foo.count() != 1366) || (foo.count() != bar.count())) {
        std::cout << "unexpected result: " << std::endl
                  << "    foo.count(): " << foo.count() << std::endl
                  << "    bar.count(): " << bar.count() << std::endl;
        return 1;
    }

    return 0;
}


int test_chrono_duration_count()
{
    typedef std::chrono::duration<int, std::milli> Milliseconds;

    Milliseconds foo(1000); // 1 sec
    foo *= 60;

    if (foo.count() != 60000) {
        std::cout << "bad duration in periods: " << foo.count() << std::endl;
        return 1;
    }

    int secs = foo.count() * Milliseconds::period::num / Milliseconds::period::den;
    if (secs != 60) {
        std::cout << "bad duration in seconds: " << secs << std::endl;
        return 1;
    }

    return 0;
}


int test_chrono_duration_min_max()
{
    auto minval = std::chrono::system_clock::duration::min().count();
    // std::cout << "min value:     " << minval << std::endl;
    // std::cout << "sys clock min: " << SYS_CLOCK_MIN << std::endl;
    if (minval != SYS_CLOCK_MIN) {
        std::cout << "bad system_clock min value: " << minval << std::endl;
        return 1;
    }

    auto maxval = std::chrono::system_clock::duration::max().count();
    if (maxval != SYS_CLOCK_MAX) {
        std::cout << "bad system_clock max value: " << maxval << std::endl;
        return 1;
    }


    return 0;
}


#else


int test_chrono_duration_ctor()
{
    return -1;
}


int test_chrono_duration_operators()
{
    return -1;
}


int test_chrono_duration_count()
{
    return -1;
}


int test_chrono_duration_min_max()
{
    return -1;
}


#endif  // GCC_ATLEAST(4, 7)
