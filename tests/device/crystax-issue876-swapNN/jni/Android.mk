LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-swap
LOCAL_SRC_FILES := main.c macro.c function.c
include $(BUILD_EXECUTABLE)
