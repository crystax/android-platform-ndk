# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE     := test-libcrystax
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CPPFLAGS   := -std=gnu++0x

LOCAL_SRC_FILES  := main.cpp \
	is_normalized.cpp \
	normalize.cpp \
	is_absolute.cpp \
	absolutize.cpp \
	is_subpath.cpp \
	relpath.cpp \
	basename.cpp \
	dirname.cpp \
	path.cpp \
	list.cpp \
	open-self.cpp \

include $(BUILD_EXECUTABLE)
