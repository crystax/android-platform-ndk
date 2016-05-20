LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := esdb-test
LOCAL_STATIC_LIBRARIES := boost_system_static boost_locale_static
LOCAL_CPPFLAGS += -std=c++14 -fexceptions -Os
LOCAL_SRC_FILES := test.cpp
include $(BUILD_EXECUTABLE)

$(call import-module,boost/1.60.0)
