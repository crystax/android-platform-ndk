LOCAL_PATH := $(call my-dir)

# Compute the compiler flags to export by the module.
# This is controlled by the APP_GNUSTL_FORCE_CPP_FEATURES variable.
# See docs/APPLICATION-MK.html for all details.
#

# Add 'rtti' feature if there was no explicit 'rtti' or 'no-rtti' value specified
ifeq (,$(filter rtti no-rtti,$(APP_GNUSTL_FORCE_CPP_FEATURES)))
    APP_GNUSTL_FORCE_CPP_FEATURES += rtti
endif
# Add 'exceptions' feature if there was no explicit 'exceptions' or 'no-exceptions' value specified
ifeq (,$(filter exceptions no-exceptions,$(APP_GNUSTL_FORCE_CPP_FEATURES)))
    APP_GNUSTL_FORCE_CPP_FEATURES += exceptions
endif

ifneq (,$(and $(filter rtti,$(APP_GNUSTL_FORCE_CPP_FEATURES)),$(filter no-rtti,$(APP_GNUSTL_FORCE_CPP_FEATURES))))
    $(error Both 'rtti' and 'no-rtti' specified in APP_GNUSTL_FORCE_CPP_FEATURES)
endif
ifneq (,$(and $(filter exceptions,$(APP_GNUSTL_FORCE_CPP_FEATURES)),$(filter no-exceptions,$(APP_GNUSTL_FORCE_CPP_FEATURES))))
    $(error Both 'exceptions' and 'no-exceptions' specified in APP_GNUSTL_FORCE_CPP_FEATURES)
endif

gnustl_exported_cppflags := $(strip \
  $(if $(filter exceptions,$(APP_GNUSTL_FORCE_CPP_FEATURES)),-fexceptions)\
  $(if $(filter no-exceptions,$(APP_GNUSTL_FORCE_CPP_FEATURES)),-fno-exceptions)\
  $(if $(filter rtti,$(APP_GNUSTL_FORCE_CPP_FEATURES)),-frtti))\
  $(if $(filter no-rtti,$(APP_GNUSTL_FORCE_CPP_FEATURES)),-fno-rtti)

# Include path to export
gnustl_exported_c_includes := $(LOCAL_PATH)/include/$(TARGET_TOOLCHAIN_VERSION) \
    $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/include

include $(CLEAR_VARS)
LOCAL_MODULE := gnustl_static
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libgnustl_static.a
LOCAL_EXPORT_CPPFLAGS := $(gnustl_exported_cppflags)
LOCAL_EXPORT_C_INCLUDES := $(gnustl_exported_c_includes)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := gnustl_shared
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libgnustl_shared.so
LOCAL_EXPORT_CPPFLAGS := $(gnustl_exported_cppflags)
LOCAL_EXPORT_C_INCLUDES := $(gnustl_exported_c_includes)
LOCAL_EXPORT_LDLIBS := $(call host-path,$(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libsupc++.a)
include $(PREBUILT_SHARED_LIBRARY)
