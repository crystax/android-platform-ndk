LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libjpeg-static
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libjpeg_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libjpeg-shared
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libjpeg_shared
include $(BUILD_EXECUTABLE)

$(call import-module,libjpeg/9a)
