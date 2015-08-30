LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := issue79115-confusing-ld.gold-warning
LOCAL_SRC_FILES := issue79115-confusing-ld.gold-warning.c
LOCAL_DISABLE_NO_EXECUTE := true
LOCAL_LDFLAGS += -Wl,--fatal-warnings

# Disable integrated assembler because -Wa,--execstack isn't supported
LOCAL_CFLAGS += $(if $(filter clang%,$(NDK_TOOLCHAIN_VERSION)),-fno-integrated-as)

include $(BUILD_EXECUTABLE)
