LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc-issue-350
LOCAL_SRC_FILES  := main.m
include $(BUILD_EXECUTABLE)
