LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-sqlite3-static
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := sqlite3_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-sqlite3-shared
LOCAL_SRC_FILES        := test.c
LOCAL_STATIC_LIBRARIES := sqlite3_shared
include $(BUILD_EXECUTABLE)

$(call import-module,sqlite3)
