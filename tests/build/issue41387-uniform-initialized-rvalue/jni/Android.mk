LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := issue41387-uniform-initialized-rvalue
LOCAL_SRC_FILES := issue41387-uniform-initialized-rvalue.cpp
include $(BUILD_EXECUTABLE)
