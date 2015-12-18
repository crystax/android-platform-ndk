LOCAL_PATH := $(call my-dir)
LOCAL_PATH_SAVED := $(LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := test
LOCAL_SRC_FILES := test.cpp
LOCAL_SHARED_LIBRARIES := libShared
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libShared
LOCAL_SRC_FILES := lib-shared.cpp
LOCAL_STATIC_LIBRARIES := libStatic
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libStatic
LOCAL_SRC_FILES := lib-static.cpp
include $(BUILD_STATIC_LIBRARY)

