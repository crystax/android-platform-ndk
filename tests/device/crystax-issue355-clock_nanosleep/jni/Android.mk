LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := clock_nanosleep
LOCAL_SRC_FILES  := main.c
include $(BUILD_EXECUTABLE)
