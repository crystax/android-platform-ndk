LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-posix_memalign
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := $(CFLAGS)
include $(BUILD_EXECUTABLE)
