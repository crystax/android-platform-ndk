LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc
LOCAL_SRC_FILES  := main.c base.m test-objc.m test-objc++.mm
include $(BUILD_EXECUTABLE)
