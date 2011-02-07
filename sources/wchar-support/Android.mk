LOCAL_PATH := $(call my-dir)

WCHAR_SRC_FILES := \
	citrus/citrus_ctype.c \
	citrus/citrus_none.c \
	citrus/citrus_utf8.c \
	locale/___runetype_mb.c \
	locale/__mb_cur_max.c \
	locale/_wctrans.c \
	locale/iswctype.c \
	locale/rune.c \
	locale/runeglue.c \
	locale/runetable.c \
	locale/wcstod.c \
	locale/wcstof.c \
	locale/wcstoimax.c \
	locale/wcstol.c \
	locale/wcstold.c \
	locale/wcstoll.c \
	locale/wcstoul.c \
	locale/wcstoull.c \
	locale/wcstoumax.c \
	locale/wctob.c \
	locale/wctomb.c \

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_static
LOCAL_SRC_FILES         := $(addprefix src/,$(WCHAR_SRC_FILES))
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include $(LOCAL_PATH)/src/locale
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_shared
LOCAL_SRC_FILES         := $(addprefix src/,$(WCHAR_SRC_FILES))
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include $(LOCAL_PATH)/src/locale
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_SHARED_LIBRARY)
