#pragma once

// First, check some macros

#if !defined(__cplusplus)
#error "__cplusplus is not defined"
#endif

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)) && __cplusplus < 201103L
#error "__cplusplus value is too small"
#endif

#if !defined(__GXX_EXPERIMENTAL_CXX0X__)
#error "__GXX_EXPERIMENTAL_CXX0X__ is not defined"
#endif

#include <iostream>

// Now, check GNU libstdc++ macros

#if defined(__GLIBCXX__) && !defined(_GLIBCXX_USE_C99)
#error "_GLIBCXX_USE_C99 is not defined"
#endif

#if defined(__GLIBCXX__) && defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF)
#error "_GLIBCXX_HAVE_BROKEN_VSWPRINTF is defined, but it shouldn't"
#endif


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


int test_language();
int test_chrono_duration_ctor();
int test_chrono_duration_operators();
int test_chrono_duration_count();
int test_chrono_duration_min_max();
int test_thread();
int test_to_string();
int test_to_wstring();
int test_stol();
