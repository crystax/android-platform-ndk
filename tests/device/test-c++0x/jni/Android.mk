# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-c++0x
LOCAL_SRC_FILES := test-c++0x.cpp
include $(BUILD_EXECUTABLE)
