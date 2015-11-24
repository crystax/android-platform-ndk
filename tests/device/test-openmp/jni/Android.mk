LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

define add-test-rule
include $$(CLEAR_VARS)
LOCAL_MODULE := $(1)
LOCAL_SRC_FILES := $(1).c
LOCAL_CFLAGS := $$(CFLAGS)
LOCAL_LDFLAGS := $$(LDFLAGS)
include $$(BUILD_EXECUTABLE)
endef

$(foreach __t,$(CTESTS),\
    $(eval $(call add-test-rule,$(__t)))\
)
