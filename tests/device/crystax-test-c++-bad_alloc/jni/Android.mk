LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-bad_alloc
LOCAL_SRC_FILES        := test.cpp
include $(BUILD_EXECUTABLE)
