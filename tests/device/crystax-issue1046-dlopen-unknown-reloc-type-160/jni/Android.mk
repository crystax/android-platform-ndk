LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := testlib
LOCAL_SRC_FILES := test.cpp
LOCAL_CPPFLAGS  := -std=c++11 -O0
LOCAL_LDLIBS    := -latomic
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := test
LOCAL_SRC_FILES := main.c
include $(BUILD_EXECUTABLE)
