LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test-static
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_STATIC_LIBRARIES := objc2rt_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-shared
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_SHARED_LIBRARIES := objc2rt_shared
include $(BUILD_EXECUTABLE)

$(call import-module,objc/gnustep-libobjc2)
