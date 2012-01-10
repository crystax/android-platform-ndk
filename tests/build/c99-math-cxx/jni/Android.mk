LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := c99-math-cxx
LOCAL_SRC_FILES := main.cpp
include $(BUILD_EXECUTABLE)
