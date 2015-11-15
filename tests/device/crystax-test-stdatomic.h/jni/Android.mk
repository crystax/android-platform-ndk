LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE    := test
LOCAL_SRC_FILES := $(SRCFILES)
LOCAL_CFLAGS    := $(CFLAGS)
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_LDLIBS    := -latomic
endif
include $(BUILD_EXECUTABLE)
