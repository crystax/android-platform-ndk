# These tests are broken.
# TODO: fix them. See also https://tracker.crystax.net/issues/932
#TESTS :=        \
#    cexp        \
#    csqrt       \
#    ctrig       \
#    exponential \
#    fenv        \
#    fma         \
#    invctrig    \
#    invtrig     \
#    logarithm   \
#    lrint       \
#    lround      \
#    nan         \
#    nearbyint   \
#    next        \
#    rem         \
#    trig        \

TESTS :=         \
	conj         \
	fmaxmin      \
	ilogb        \

CFLAGS := -fno-builtin
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-value
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -UNDEBUG
