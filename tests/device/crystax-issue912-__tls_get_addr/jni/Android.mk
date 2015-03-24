LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := tga
LOCAL_SRC_FILES := tga.c
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := test-tls-get-addr
LOCAL_SRC_FILES := test.c
LOCAL_SHARED_LIBRARIES := tga
include $(BUILD_EXECUTABLE)
