LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := issue-440-big-switch
LOCAL_SRC_FILES := main.c
include $(BUILD_SHARED_LIBRARY)
