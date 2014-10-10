LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := crystax-test-headers
LOCAL_SRC_FILES := test.c test-c++.cpp test-objc++.mm
ifeq (,$(filter %.hpp,$(HEADER)))
LOCAL_SRC_FILES += test-c.c test-objc.m
endif
include $(BUILD_EXECUTABLE)
