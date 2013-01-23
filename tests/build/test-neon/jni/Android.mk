LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-neon
LOCAL_ARM_NEON  := true
LOCAL_SRC_FILES := main.cpp
include $(BUILD_SHARED_LIBRARY)
