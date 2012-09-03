LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-neon-on-4.6
LOCAL_ARM_NEON  := true
LOCAL_SRC_FILES := main.cpp
include $(BUILD_EXECUTABLE)
