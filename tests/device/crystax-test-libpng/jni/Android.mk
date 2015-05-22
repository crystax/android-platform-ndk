LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libpng-static
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libpng_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libpng-shared
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libpng_shared
include $(BUILD_EXECUTABLE)

$(call import-module,libpng/1.6.17)
