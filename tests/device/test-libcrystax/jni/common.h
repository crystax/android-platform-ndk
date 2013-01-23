#ifndef TEST_LIBCRYSTAX_48f8fbd909ef410d9d798cfacbd1e580
#define TEST_LIBCRYSTAX_48f8fbd909ef410d9d798cfacbd1e580

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <crystax.h>
#include <crystax/common.hpp>

int test_is_normalized();
int test_normalize();
int test_is_absolute();
int test_absolutize();
int test_is_subpath();
int test_relpath();
int test_basename();
int test_dirname();
int test_path();
int test_list();
int test_open_self();

#endif /* TEST_LIBCRYSTAX_48f8fbd909ef410d9d798cfacbd1e580 */
