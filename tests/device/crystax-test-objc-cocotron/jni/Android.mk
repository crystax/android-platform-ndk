LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

include $(CLEAR_VARS)
LOCAL_MODULE     := test
LOCAL_SRC_FILES  := $(SRCFILES)
LOCAL_SHARED_LIBRARIES := Foundation CoreFoundation CoreServices CFNetwork objc2rt_shared
include $(BUILD_EXECUTABLE)

$(call import-module,objc/cocotron/0.1.0)
$(call import-module,objc/gnustep-libobjc2)
