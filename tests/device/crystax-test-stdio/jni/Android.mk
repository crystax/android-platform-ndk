LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)

LOCAL_MODULE := test-stdio

LOCAL_SRC_FILES := $(SRCFILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS := -Werror
LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
