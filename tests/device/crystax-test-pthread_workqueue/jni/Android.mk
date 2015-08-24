LOCAL_PATH := $(call my-dir)

ifeq (,$(strip $(NDK)))
NDK := $(abspath $(LOCAL_PATH)/../../../..)
endif

TDIR := $(NDK)/sources/crystax/tests/libpwq

CFLAGS := -Wall -Wextra -Werror
CFLAGS += -DNO_CONFIG_H

include $(CLEAR_VARS)
LOCAL_MODULE     := crystax-test-pthread_workqueue-1
LOCAL_SRC_FILES  := $(TDIR)/api/test.c
LOCAL_CFLAGS     := $(CFLAGS)
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE     := crystax-test-pthread_workqueue-2
LOCAL_SRC_FILES  := $(TDIR)/idle/main.c
LOCAL_CFLAGS     := $(CFLAGS)
include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)
#LOCAL_MODULE     := crystax-test-pthread_workqueue-3
#LOCAL_SRC_FILES  := $(TDIR)/latency/latency.c
#LOCAL_CFLAGS     := $(CFLAGS)
#include $(BUILD_EXECUTABLE)
