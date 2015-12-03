LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-alloca
LOCAL_SRC_FILES := main.c
LOCAL_CFLAGS    := -Wall -Wextra -Werror
include $(BUILD_EXECUTABLE)
