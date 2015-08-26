CTESTS :=            \
	fmemopen         \
	fopen            \
	getdelim         \
	getline          \
	open_memstream   \
	open_wmemstream  \
	popen            \
	print-positional \
	printbasic       \
	printf           \
	printfloat       \
	scanfloat        \

CFLAGS := -Wall -Wextra -Werror
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-variable
CFLAGS += -UNDEBUG
