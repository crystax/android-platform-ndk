CTESTS :=   \
	openmp  \
	openmp2 \
	fib     \

CFLAGS  := -fopenmp
LDFLAGS := -fopenmp

CFLAGS  += -Wall -Wextra -Werror
