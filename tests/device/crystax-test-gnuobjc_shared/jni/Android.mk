LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc
LOCAL_SRC_FILES  := $(SRCFILES)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := test-objc-dlopen-libgnuobjc
LOCAL_SRC_FILES := test-dlopen-libgnuobjc.c
LOCAL_SHARED_LIBRARIES := libgnuobjc_shared
include $(BUILD_EXECUTABLE)
