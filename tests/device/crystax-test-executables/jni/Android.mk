LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := dynamic-executable
LOCAL_SRC_FILES := main.c
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := static-executable
LOCAL_SRC_FILES := main.c
LOCAL_LDFLAGS += -static
include $(BUILD_EXECUTABLE)
