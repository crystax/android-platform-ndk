LOCAL_PATH := $(call my-dir)

ifeq ($(strip $(filter-out $(NDK_KNOWN_ARCHS),$(TARGET_ARCH))),)
include $(CLEAR_VARS)
LOCAL_MODULE := ssax_instruction
LOCAL_ARM_NEON := true
LOCAL_SRC_FILES := test.S
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_CFLAGS := -fno-integrated-as
endif
include $(BUILD_SHARED_LIBRARY)
endif
