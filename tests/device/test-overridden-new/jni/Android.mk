LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-overridden-new
LOCAL_SRC_FILES := main.cpp
include $(BUILD_EXECUTABLE)
