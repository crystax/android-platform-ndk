LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-popen
LOCAL_SRC_FILES := test.c
include $(BUILD_EXECUTABLE)
