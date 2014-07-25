# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := crystax-test-c++11
LOCAL_SRC_FILES := main.cpp                 \
                   test-chrono-duration.cpp \
                   test-thread.cpp          \
                   test-to-string.cpp       \
                   test-to-wstring.cpp      \
                   test-stol.cpp
include $(BUILD_EXECUTABLE)
