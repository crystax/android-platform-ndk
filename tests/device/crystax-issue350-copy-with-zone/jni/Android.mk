LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc-issue-350
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_CFLAGS     := $(CFLAGS)
ifeq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_CFLAGS     += -Wno-unused-parameter
endif
include $(BUILD_EXECUTABLE)
