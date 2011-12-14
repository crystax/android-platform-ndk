LOCAL_PATH := $(call my-dir)

CRYSTAX_FORCE_REBUILD := $(strip $(CRYSTAX_FORCE_REBUILD))
ifndef CRYSTAX_FORCE_REBUILD
  ifeq (,$(strip $(wildcard $(LOCAL_PATH)/libs/armeabi/$(TARGET_TOOLCHAIN_VERSION)/libcrystax.a)))
    $(call __ndk_info,WARNING: Rebuilding crystax libraries from sources!)
    $(call __ndk_info,You might want to use $$NDK/build/tools/build-crystax.sh)
    $(call __ndk_info,in order to build prebuilt versions to speed up your builds!)
    CRYSTAX_FORCE_REBUILD := true
  endif
endif

CRYSTAX_SRC_FILES := \
	android/locale/UTF-8.LC_CTYPE.c \
	android/locale/el_GR.ISO8859-7.LC_CTYPE.c \
	android/locale/la_LN.ISO8859-1.LC_CTYPE.c \
	android/locale/la_LN.US-ASCII.LC_CTYPE.c \
	android/utils.c \
	locale/ascii.c \
	locale/big5.c \
	locale/btowc.c \
	locale/collate.c \
	locale/euc.c \
	locale/fix_grouping.c \
	locale/gb18030.c \
	locale/gb2312.c \
	locale/gbk.c \
	locale/isctype.c \
	locale/iswctype.c \
	locale/ldpart.c \
	locale/lmessages.c \
	locale/lmonetary.c \
	locale/lnumeric.c \
	locale/localeconv.c \
	locale/mblen.c \
	locale/mbrlen.c \
	locale/mbrtowc.c \
	locale/mbsinit.c \
	locale/mbsnrtowcs.c \
	locale/mbsrtowcs.c \
	locale/mbstowcs.c \
	locale/mbtowc.c \
	locale/mskanji.c \
	locale/nextwctype.c \
	locale/none.c \
	locale/rune.c \
	locale/runetype.c \
	locale/setlocale.c \
	locale/setrunelocale.c \
	locale/table.c \
	locale/tolower.c \
	locale/toupper.c \
	locale/utf8.c \
	locale/wcrtomb.c \
	locale/wcsftime.c \
	locale/wcsnrtombs.c \
	locale/wcsrtombs.c \
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
	locale/wctrans.c \
	locale/wctype.c \
	locale/wcwidth.c \
	stdtime/timelocal.c \
	string/memchr.c \
	string/wcpcpy.c \
	string/wcpncpy.c \
	string/wcscasecmp.c \
	string/wcscat.c \
	string/wcschr.c \
	string/wcscmp.c \
	string/wcscoll.c \
	string/wcscpy.c \
	string/wcscspn.c \
	string/wcsdup.c \
	string/wcslcat.c \
	string/wcslcpy.c \
	string/wcslen.c \
	string/wcsncasecmp.c \
	string/wcsncat.c \
	string/wcsncmp.c \
	string/wcsncpy.c \
	string/wcsnlen.c \
	string/wcspbrk.c \
	string/wcsrchr.c \
	string/wcsspn.c \
	string/wcsstr.c \
	string/wcstok.c \
	string/wcswidth.c \
	string/wcsxfrm.c \
	string/wmemchr.c \
	string/wmemcmp.c \
	string/wmemcpy.c \
	string/wmemmove.c \
	string/wmemset.c \

ifneq ($(CRYSTAX_FORCE_REBUILD),true)

$(call ndk_log,Using prebuilt crystax libraries)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_static
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystax.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_shared
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystax.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

else # CRYSTAX_FORCE_REBUILD == true

$(call ndk_log,Rebuilding crystax libraries from sources)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_static
LOCAL_MODULE_FILENAME   := libcrystax
LOCAL_SRC_FILES         := $(addprefix src/,$(CRYSTAX_SRC_FILES))
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include $(LOCAL_PATH)/src/locale $(LOCAL_PATH)/src/android
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_shared
LOCAL_MODULE_FILENAME   := libcrystax
LOCAL_SRC_FILES         := $(addprefix src/,$(CRYSTAX_SRC_FILES))
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include $(LOCAL_PATH)/src/locale $(LOCAL_PATH)/src/android
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_SHARED_LIBRARY)

endif # CRYSTAX_FORCE_REBUILD == true
