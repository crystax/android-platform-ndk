LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test_wchar

LOCAL_SRC_FILES := main.c \
	test-btowc.c \
	test-iswctype.c \
	test-mblen.c \
	test-mbrlen.c \
	test-mbrtowc.c \
	test-mbsnrtowcs.c \
	test-mbsrtowcs.c \
	test-mbstowcs.c \
	test-mbtowc.c \
	test-towctrans.c \
	test-wcrtomb.c \
	test-wcscasecmp.c \
	test-wcsnlen.c \
	test-wcsnrtombs.c \
	test-wcsrtombs.c \
	test-wcstombs.c \
	test-wctomb.c \

LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_EXECUTABLE)
