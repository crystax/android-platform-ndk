LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-static
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_STATIC_LIBRARIES := objc2rt_static
LOCAL_CFLAGS     := $(CFLAGS)
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_OBJCFLAGS  := -fno-objc-arc
endif
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-shared
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_SHARED_LIBRARIES := objc2rt_shared
LOCAL_CFLAGS     := $(CFLAGS)
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_OBJCFLAGS  := -fno-objc-arc
endif
include $(BUILD_EXECUTABLE)

$(call import-module,objc/gnustep-libobjc2)
