LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := objc2rt_static
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libobjc.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := objc2rt_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libobjc.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := objcxx2rt
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libobjcxx.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)
