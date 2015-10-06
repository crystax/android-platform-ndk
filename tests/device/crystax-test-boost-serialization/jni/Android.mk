LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-boost-static
LOCAL_SRC_FILES        := test.cpp gps.cpp
LOCAL_STATIC_LIBRARIES := boost_serialization_static
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-boost-shared
LOCAL_SRC_FILES        := test.cpp gps.cpp
LOCAL_STATIC_LIBRARIES := boost_serialization_shared
include $(BUILD_EXECUTABLE)

$(call import-module,boost/1.59.0)
