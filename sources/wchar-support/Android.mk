LOCAL_PATH := $(call my-dir)

WCHAR_FORCE_REBUILD := $(strip $(WCHAR_FORCE_REBUILD))
ifndef WCHAR_FORCE_REBUILD
  ifeq (,$(strip $(wildcard $(LOCAL_PATH)/libs/armeabi/libwchar_static.a)))
    $(call __ndk_info,WARNING: Rebuilding wchar support libraries from sources!)
    $(call __ndk_info,You might want to use $$NDK/build/tools/build-wchar-support.sh)
    $(call __ndk_info,in order to build prebuilt versions to speed up your builds!)
    WCHAR_FORCE_REBUILD := true
  endif
endif

WCHAR_SRC_FILES := \
	citrus/citrus_ctype.c \
	citrus/citrus_none.c \
	citrus/citrus_utf8.c \
	locale/___runetype_mb.c \
	locale/__mb_cur_max.c \
	locale/_wctrans.c \
	locale/iswctype.c \
	locale/mbstowcs.c \
	locale/multibyte_citrus.c \
	locale/rune.c \
	locale/runeglue.c \
	locale/runetable.c \
	locale/wcstod.c \
	locale/wcstof.c \
	locale/wcstoimax.c \
	locale/wcstol.c \
	locale/wcstoll.c \
	locale/wcstombs.c \
	locale/wcstoul.c \
	locale/wcstoull.c \
	locale/wcstoumax.c \
	locale/wctob.c \
	locale/wctomb.c \

ifneq ($(WCHAR_FORCE_REBUILD),true)

$(call ndk_log,Using prebuilt wchar support libraries)

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_static
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE).a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_shared
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE).so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

else # WCHAR_FORCE_REBUILD == true

$(call ndk_log,Rebuilding wchar support libraries from sources)

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

endif # WCHAR_FORCE_REBUILD == true
