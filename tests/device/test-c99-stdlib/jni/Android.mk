LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test-c99-stdlib

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := main.c

include $(BUILD_EXECUTABLE)
