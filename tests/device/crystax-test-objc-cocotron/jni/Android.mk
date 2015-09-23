LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../common.mk

define add-test-case
include $(CLEAR_VARS)
LOCAL_MODULE     := test-cocotron-$$(if $$(filter arc,$(2)),arc,noarc)-$(1)
LOCAL_SRC_FILES  := test-$(1).m
LOCAL_C_INCLUDES := $$(LOCAL_PATH)
LOCAL_CFLAGS     := $$(CFLAGS)
LOCAL_CFLAGS     += $$(if $$(filter arc,$(2)),-fobjc-arc,-fno-objc-arc)
include $(BUILD_EXECUTABLE)
endef

$(foreach __t,$(CTESTS),\
    $(foreach __a,noarc arc,\
        $(eval $(call add-test-case,$(__t),$(__a)))\
    )\
)
