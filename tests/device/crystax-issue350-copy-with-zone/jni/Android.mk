LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc-issue-350
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_CFLAGS     := $(CFLAGS)
LOCAL_CFLAGS     += -Wno-unused-parameter
include $(BUILD_EXECUTABLE)
