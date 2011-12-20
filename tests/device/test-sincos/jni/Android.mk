# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-sincos
LOCAL_SRC_FILES := test-sincos-main.c test-sincos-c.c test-sincos-cpp.cpp
include $(BUILD_EXECUTABLE)
