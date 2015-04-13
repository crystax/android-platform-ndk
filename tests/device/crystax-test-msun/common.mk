TESTS :=        \
	cexp        \
	conj        \
	csqrt       \
	ctrig       \
	exponential \
	fenv        \
	fma         \
	fmaxmin     \
	ilogb       \
	invctrig    \
	invtrig     \
	logarithm   \
	lrint       \
	lround      \
	nan         \
	nearbyint   \
	next        \
	rem         \
	trig        \

CFLAGS := -fno-builtin
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-value
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -UNDEBUG
