LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE := crystax-test-c++11
LOCAL_CFLAGS := $(CFLAGS)
LOCAL_SRC_FILES := $(SRCFILES)
include $(BUILD_EXECUTABLE)
