LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := test-boost1.57.0-dependencies
LOCAL_SRC_FILES := test.cpp
LOCAL_STATIC_LIBRARIES := boost_regex_shared
LOCAL_STATIC_LIBRARIES += boost_exception_static
include $(BUILD_EXECUTABLE)

$(call import-module,boost+icu/1.57.0)
