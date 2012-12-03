LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test_printf

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_SRC_FILES := main.c        \
	           test-printf.c

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
