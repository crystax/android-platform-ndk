# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

TEST_LIBCRYSTAX_VFS := false

include $(CLEAR_VARS)
LOCAL_MODULE     := test-libcrystax
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CPPFLAGS   := -std=gnu++0x

LOCAL_SRC_FILES  := main.cpp \
	list.cpp \
	open-self.cpp \

ifeq ($(TEST_LIBCRYSTAX_VFS),true)

LOCAL_STATIC_LIBRARIES += crystaxvfs_static
LOCAL_CFLAGS += -DTEST_LIBCRYSTAXVFS=1

LOCAL_SRC_FILES += \
    dirname.cpp \
    relpath.cpp \
    basename.cpp \
    is_subpath.cpp \
    absolutize.cpp \
    path.cpp \
    is_absolute.cpp \
    normalize.cpp \
    is_normalized.cpp \

endif

include $(BUILD_EXECUTABLE)
