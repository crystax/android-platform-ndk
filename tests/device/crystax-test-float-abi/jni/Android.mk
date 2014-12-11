LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-float-abi
LOCAL_SRC_FILES := test-float.c
include $(BUILD_EXECUTABLE)
