LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := std-atomic
LOCAL_SRC_FILES  := test.cpp
ifneq ($(filter clang%,$(NDK_TOOLCHAIN_VERSION)),)
LOCAL_LDLIBS     := -latomic
endif
include $(BUILD_EXECUTABLE)
