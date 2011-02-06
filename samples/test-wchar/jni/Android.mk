LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := test-wchar
LOCAL_SRC_FILES := test-wchar.c \
	test-c.c test-cpp.cpp

LOCAL_STATIC_LIBRARIES := wchar-support

include $(BUILD_EXECUTABLE)

$(call import-module,wchar-support)
