# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-c++11
LOCAL_SRC_FILES := main.cpp                 \
                   test-chrono-duration.cpp \
                   test-thread.cpp
include $(BUILD_EXECUTABLE)
