LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := test
LOCAL_SRC_FILES  := main.c
LOCAL_CFLAGS     := -Wall -Wextra -Werror
LOCAL_CFLAGS     += -UNDEBUG
include $(BUILD_EXECUTABLE)
