# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-stream-float-output
LOCAL_SRC_FILES := test-stream-float-output.cpp
include $(BUILD_EXECUTABLE)
