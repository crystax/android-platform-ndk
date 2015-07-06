LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libtiff-static
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libtiff_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-libtiff-shared
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := libtiff_shared
include $(BUILD_EXECUTABLE)

$(call import-module,libtiff/4.0.4)
