LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

define add-test-case
include $(CLEAR_VARS)
LOCAL_MODULE     := test-cocotron-$(1)
LOCAL_SRC_FILES  := test-$(1).m
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_CFLAGS     := $(CFLAGS)
include $(BUILD_EXECUTABLE)
endef

$(foreach __t,$(CTESTS),\
    $(eval $(call add-test-case,$(__t)))\
)
