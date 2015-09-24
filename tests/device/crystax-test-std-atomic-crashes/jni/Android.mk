LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := test_std_atomic_crash_gnustl_static
LOCAL_SRC_FILES        := main.cc
LOCAL_STATIC_LIBRARIES := gnustl_static
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_LDLIBS := -latomic
endif
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE           := test_std_atomic_crash_gnustl_shared
LOCAL_SRC_FILES        := main.cc
LOCAL_SHARED_LIBRARIES := gnustl_shared
ifneq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
LOCAL_LDLIBS := -latomic
endif
include $(BUILD_EXECUTABLE)

$(call import-module,cxx-stl/gnu-libstdc++)
