LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc
LOCAL_SRC_FILES  := main.c base.m test-objc.m test-objc++.mm
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := test-dlopen-libgnuobjc
LOCAL_SRC_FILES := test-dlopen-libgnuobjc.c
LOCAL_SHARED_LIBRARIES := libgnuobjc_shared
include $(BUILD_EXECUTABLE)
