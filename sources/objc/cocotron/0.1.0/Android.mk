LOCAL_PATH := $(call my-dir)

define add-framework
ifneq (,$(filter $(2),$(TARGET_ARCH_ABI)))
include $(CLEAR_VARS)
LOCAL_MODULE := $(1)
LOCAL_SRC_FILES := frameworks/$(TARGET_ARCH_ABI)/$(1).framework/Versions/Current/lib$(1).so
LOCAL_EXPORT_CFLAGS := -F$(LOCAL_PATH)/frameworks/$(TARGET_ARCH_ABI) -framework $(1)
sinclude $(LOCAL_PATH)/frameworks/$(TARGET_ARCH_ABI)/$(1).framework/deps.mk
LOCAL_SHARED_LIBRARIES += objc2rt_shared
LOCAL_EXPORT_LDLIBS := -llog
include $(PREBUILT_SHARED_LIBRARY)
endif
endef

$(foreach __abi,\
    $(foreach __dir,$(wildcard $(LOCAL_PATH)/frameworks/*),$(notdir $(__dir)) ),\
    $(foreach __f,$(wildcard $(LOCAL_PATH)/frameworks/$(__abi)/*.framework),\
        $(eval $(call add-framework,$(patsubst %.framework,%,$(notdir $(__f))),$(__abi)))\
    )\
)

$(call import-module,objc/gnustep-libobjc2)
