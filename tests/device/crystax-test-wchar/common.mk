TESTS :=       \
	btowc      \
	iswctype   \
	mblen      \
	mbrlen     \
	mbrtowc    \
	mbsnrtowcs \
	mbsrtowcs  \
	mbstowcs   \
	mbtowc     \
	towctrans  \
	wcrtomb    \
	wcscasecmp \
	wcslen     \
	wcsnlen    \
	wcsnrtombs \
	wcsrtombs  \
	wcstombs   \
	wctomb     \

CXXTESTS :=    \
	wstring    \
	wcslen-cpp \

CFLAGS := -Wall -Wextra -Werror
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-variable
CFLAGS += -UNDEBUG
