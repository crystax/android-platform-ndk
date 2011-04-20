LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := build_wchar_static
LOCAL_SRC_FILES        := test-wchar.c test-c.c test-cpp.cpp
LOCAL_STATIC_LIBRARIES := wchar_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := build_wchar_shared
LOCAL_SRC_FILES        := test-wchar.c test-c.c test-cpp.cpp
LOCAL_SHARED_LIBRARIES := wchar_shared
include $(BUILD_EXECUTABLE)

$(call import-module,wchar-support)
