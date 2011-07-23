LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := build_crystax_static
LOCAL_SRC_FILES        := build-crystax.c
LOCAL_STATIC_LIBRARIES := crystax_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := build_crystax_shared
LOCAL_SRC_FILES        := build-crystax.c
LOCAL_SHARED_LIBRARIES := crystax_shared
include $(BUILD_EXECUTABLE)

