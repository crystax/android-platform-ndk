LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := crystax-test-c99-complex
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SRC_FILES := main.c
LOCAL_CFLAGS := -fno-builtin
include $(BUILD_EXECUTABLE)
