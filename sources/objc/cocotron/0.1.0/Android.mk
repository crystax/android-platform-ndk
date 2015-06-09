LOCAL_PATH := $(call my-dir)

define add-framework
ifneq (,$(filter $(2),$(TARGET_ARCH_ABI)))
include $(CLEAR_VARS)
LOCAL_MODULE := $(1)
LOCAL_SRC_FILES := frameworks/$(TARGET_ARCH_ABI)/$(1).framework/Versions/Current/lib$(1).so
LOCAL_EXPORT_CFLAGS := -F$(LOCAL_PATH)/frameworks/$(TARGET_ARCH_ABI) -framework $(1)
include $(PREBUILT_SHARED_LIBRARY)
endif
endef

$(foreach __abi,\
    $(foreach __dir,$(wildcard $(LOCAL_PATH)/frameworks/*),$(notdir $(__dir)) ),\
    $(foreach __f,$(wildcard $(LOCAL_PATH)/frameworks/$(__abi)/*.framework),\
        $(eval $(call add-framework,$(patsubst %.framework,%,$(notdir $(__f))),$(__abi)))\
    )\
)

#$(foreach __f,\
#    Foundation CFNetwork CoreServices,\
#    $(eval $(call add-framework,$(__f)))\
#)
