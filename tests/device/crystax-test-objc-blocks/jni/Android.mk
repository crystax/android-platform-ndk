LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-objc-blocks
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_CFLAGS     := -fblocks
endif
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_STATIC_LIBRARIES := compiler_rt_static
include $(BUILD_EXECUTABLE)

$(call import-module,android/compiler-rt)
