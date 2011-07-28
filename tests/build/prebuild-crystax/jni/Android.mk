LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := build_crystax
LOCAL_SRC_FILES        := build-crystax.c
LOCAL_STATIC_LIBRARIES := crystax
include $(BUILD_EXECUTABLE)
