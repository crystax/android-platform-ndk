LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test
LOCAL_SRC_FILES        := test.cpp
LOCAL_STATIC_LIBRARIES := boost_regex_shared
include $(BUILD_EXECUTABLE)

$(call import-module,boost+icu/1.58.0)
