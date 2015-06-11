LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test
LOCAL_SRC_FILES  := $(SRCFILES)
include $(BUILD_EXECUTABLE)
