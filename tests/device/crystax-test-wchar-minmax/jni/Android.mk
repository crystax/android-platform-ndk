LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := crystax-test-wchar-minmax
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_CFLAGS     := $(CFLAGS)
include $(BUILD_EXECUTABLE)
