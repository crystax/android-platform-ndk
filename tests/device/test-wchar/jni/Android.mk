LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test_wchar

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := main.c \
	test-wcslen-c.c \
	test-wcslen-cpp.cpp \
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
	test-wstring.cpp \

LOCAL_STATIC_LIBRARIES := wchar_static

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
