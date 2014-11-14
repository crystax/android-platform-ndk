LOCAL_PATH := $(call my-dir)

$(shell $(LOCAL_PATH)/gen 20000)

include $(CLEAR_VARS)
LOCAL_MODULE := crystax-test-big-switch
LOCAL_SRC_FILES := main.c switch.c
include $(BUILD_EXECUTABLE)
