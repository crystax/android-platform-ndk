LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test_wchar_minmax

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := main.c

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
