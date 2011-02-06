LOCAL_PATH := $(call my-dir)

WCHAR_SRC_FILES :=

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_static
LOCAL_SRC_FILES         := $(addprefix src/,$(WCHAR_SRC_FILES))
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := wchar_shared
LOCAL_SRC_FILES         := $(addprefix src/,$(WCHAR_SRC_FILES))
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(BUILD_SHARED_LIBRARY)
