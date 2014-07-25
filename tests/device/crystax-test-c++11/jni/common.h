#pragma once

#include <iostream>


#define DO_TEST(name)   switch (test_ ## name ()) {                   \
                        case 0:                                       \
                            std::cout << "test " #name ": OK\n";      \
                            break;                                    \
                        case -1:                                      \
                            std::cout << "test " #name ": SKIPPED\n"; \
                            break;                                    \
                        default:                                      \
                            std::cout << "FAILED test " #name "\n";   \
                            return 1;                                 \
                        }


#define GCC_ATLEAST(major, minor) \
    (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))


int test_chrono_duration_ctor();
int test_chrono_duration_operators();
int test_chrono_duration_count();
int test_chrono_duration_min_max();
int test_thread();
int test_to_string();
int test_to_wstring();
int test_stol();
