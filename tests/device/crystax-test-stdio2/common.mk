CTESTS :=            \
	fmemopen         \
	fopen            \
	getdelim         \
	open_memstream   \
	open_wmemstream  \
	print-positional \
	printbasic       \
	printfloat       \
	scanfloat        \

CFLAGS := -Wall -Wextra -Werror
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-variable
CFLAGS += -UNDEBUG
