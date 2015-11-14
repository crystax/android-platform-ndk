LOCAL_PATH := $(call my-dir)

ifeq (,$(strip $(NDK)))
$(error NDK is not defined!)
endif

ifeq (,$(wildcard $(NDK)))
$(error NDK points to non-existent location: '$(NDK)')
endif

BIONIC := $(realpath $(NDK)/sources/crystax/tests/bionic)

ifeq (,$(strip $(BIONIC)))
$(error Can not locate Bionic tests!)
endif

GTEST := $(NDK)/sources/third_party/googletest/googletest
ifeq (,$(wildcard $(GTEST)))
$(error Can not locate Google Test sources!)
endif

include $(CLEAR_VARS)
LOCAL_MODULE        := gtest
LOCAL_SRC_FILES     := $(GTEST)/src/gtest-all.cc
LOCAL_SRC_FILES     += $(GTEST)/src/gtest_main.cc
#LOCAL_SRC_FILES     += $(BIONIC)/gtest_main.cpp
LOCAL_CPPFLAGS      := -Wall -Werror
LOCAL_CPPFLAGS      += -I$(GTEST)/include
LOCAL_CPPFLAGS      += -I$(GTEST)
LOCAL_EXPORT_CFLAGS := -I$(GTEST)/include
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE           := test-bionic
LOCAL_SRC_FILES        := $(wildcard $(BIONIC)/*_test.cpp)
LOCAL_SRC_FILES        += $(wildcard $(BIONIC)/*_tests.cpp)
LOCAL_CPPFLAGS         := -Wall -Wextra -Wunused -Werror
LOCAL_CPPFLAGS         += -fstack-protector-all
LOCAL_CPPFLAGS         += -fno-builtin
LOCAL_CPPFLAGS         += -DUSE_DLMALLOC
LOCAL_STATIC_LIBRARIES := gtest
include $(BUILD_EXECUTABLE)
