LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-dynamic-cast
LOCAL_SRC_FILES := main.cpp 

include $(BUILD_EXECUTABLE)
