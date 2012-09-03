LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-objc
LOCAL_SRC_FILES := main.c test-objc.m test-objc++.mm
include $(BUILD_EXECUTABLE)
