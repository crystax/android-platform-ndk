#
# Copyright (c) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
# 
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
# 
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY Dmitry Moskalchuk ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Dmitry Moskalchuk OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of Dmitry Moskalchuk.
#

LOCAL_PATH := $(call my-dir)

CRYSTAX_FORCE_REBUILD := $(strip $(CRYSTAX_FORCE_REBUILD))
ifndef CRYSTAX_FORCE_REBUILD
  ifeq (,$(strip $(wildcard $(LOCAL_PATH)/libs/armeabi/$(TARGET_TOOLCHAIN_VERSION)/libcrystax_static.a)))
    $(call __ndk_info,WARNING: Rebuilding crystax libraries from sources!)
    $(call __ndk_info,You might want to use $$NDK/build/tools/build-crystax.sh)
    $(call __ndk_info,in order to build prebuilt versions to speed up your builds!)
    CRYSTAX_FORCE_REBUILD := true
  endif
endif

CRYSTAX_VFS_FORCE_REBUILD := $(strip $(CRYSTAX_VFS_FORCE_REBUILD))
ifndef CRYSTAX_VFS_FORCE_REBUILD
  ifeq (,$(strip $(wildcard $(LOCAL_PATH)/libs/armeabi/$(TARGET_TOOLCHAIN_VERSION)/libcrystaxvfs_static.a)))
    #$(call __ndk_info,WARNING: Rebuilding crystax vfs libraries from sources!)
    #$(call __ndk_info,You might want to use $$NDK/build/tools/build-crystax-vfs.sh)
    #$(call __ndk_info,in order to build prebuilt versions to speed up your builds!)
    CRYSTAX_VFS_FORCE_REBUILD := true
  endif
endif

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_empty
LOCAL_MODULE_FILENAME   := libcrystax
LOCAL_SRC_FILES         :=
include $(BUILD_STATIC_LIBRARY)

CRYSTAX_LDLIBS := -llog

CRYSTAX_INTERNAL_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(wildcard $(LOCAL_PATH)/src/*)

CRYSTAX_INTERNAL_INCLUDES += $(LOCAL_PATH)/src/include/$(TARGET_ARCH)
CRYSTAX_INTERNAL_INCLUDES += $(LOCAL_PATH)/../cxx-stl/system/include

CRYSTAX_C_WARNINGS   := -Wall -Wextra -Wno-unused
CRYSTAX_CPP_WARNINGS := -Wnon-template-friend -Woverloaded-virtual -Wsign-promo

CRYSTAX_CFLAGS       := -DCRYSTAX=1
#CRYSTAX_CFLAGS       += -DCRYSTAX_DEBUG=1
CRYSTAX_CFLAGS       += -DCRYSTAX_INIT_DEBUG=1
CRYSTAX_CFLAGS       += -DCRYSTAX_FILEIO_DEBUG=1
#CRYSTAX_CFLAGS       += -DCRYSTAX_DEBUG_PATH_FUNCTIONS=1
CRYSTAX_CFLAGS       += $(CRYSTAX_C_WARNINGS)

CRYSTAX_CPPFLAGS     := -std=gnu++0x
CRYSTAX_CPPFLAGS     += -fno-exceptions -fno-rtti
CRYSTAX_CPPFLAGS     += $(CRYSTAX_CPP_WARNINGS)

#======================================================================================================
# CrystaX libraries

ifneq ($(CRYSTAX_FORCE_REBUILD),true)

$(call ndk_log,Using prebuilt crystax libraries)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_static
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystax_static.a
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_shared
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystax_shared.so
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(PREBUILT_SHARED_LIBRARY)

else # CRYSTAX_FORCE_REBUILD == true

$(call ndk_log,Rebuilding crystax libraries from sources)

CRYSTAX_C_SRC_FILES   := $(shell cd $(LOCAL_PATH) && find src -name '*.c' -print)
CRYSTAX_CPP_SRC_FILES := $(shell cd $(LOCAL_PATH) && find src -name '*.cpp' -a -not -name 'android_jni.cpp' -print)
CRYSTAX_SRC_FILES     := $(CRYSTAX_C_SRC_FILES) $(CRYSTAX_CPP_SRC_FILES)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_static
LOCAL_SRC_FILES         := $(CRYSTAX_SRC_FILES)
LOCAL_C_INCLUDES        := $(CRYSTAX_INTERNAL_INCLUDES)
LOCAL_CFLAGS            := $(CRYSTAX_CFLAGS)
LOCAL_CPPFLAGS          := $(CRYSTAX_CPPFLAGS)
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_CPPFLAGS   := -std=gnu++0x
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystax_shared
LOCAL_SRC_FILES         := $(CRYSTAX_SRC_FILES) src/crystax/android_jni.cpp
LOCAL_C_INCLUDES        := $(CRYSTAX_INTERNAL_INCLUDES)
LOCAL_CFLAGS            := $(CRYSTAX_CFLAGS)
LOCAL_CPPFLAGS          := $(CRYSTAX_CPPFLAGS)
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_CPPFLAGS   := -std=gnu++0x
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(BUILD_SHARED_LIBRARY)

endif # CRYSTAX_FORCE_REBUILD == true

#======================================================================================================
# CrystaX VFS libraries

ifneq ($(CRYSTAX_VFS_FORCE_REBUILD),true)

$(call ndk_log,Using prebuilt crystax vfs libraries)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystaxvfs_static
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystaxvfs_static.a
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/vfs/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystaxvfs_shared
LOCAL_SRC_FILES         := libs/$(TARGET_ARCH_ABI)/$(TARGET_TOOLCHAIN_VERSION)/libcrystaxvfs_shared.so
LOCAL_SHARED_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/vfs/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(PREBUILT_SHARED_LIBRARY)

else # CRYSTAX_VFS_FORCE_REBUILD == true

$(call ndk_log,Rebuilding crystax vfs libraries from sources)

CRYSTAX_VFS_C_SRC_FILES   := $(shell cd $(LOCAL_PATH) && find vfs -name '*.c' -print)
CRYSTAX_VFS_CPP_SRC_FILES := $(shell cd $(LOCAL_PATH) && find vfs -name '*.cpp' -a -not -name 'android_jni.cpp' -print)
CRYSTAX_VFS_SRC_FILES     := $(CRYSTAX_VFS_C_SRC_FILES) $(CRYSTAX_VFS_CPP_SRC_FILES)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystaxvfs_static
LOCAL_SRC_FILES         := $(CRYSTAX_VFS_SRC_FILES)
LOCAL_C_INCLUDES        := $(CRYSTAX_INTERNAL_INCLUDES) $(LOCAL_PATH)/vfs $(LOCAL_PATH)/vfs/include
LOCAL_CFLAGS            := $(CRYSTAX_CFLAGS)
LOCAL_CPPFLAGS          := $(CRYSTAX_CPPFLAGS)
LOCAL_STATIC_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_CPPFLAGS   := -std=gnu++0x
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/vfs/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := crystaxvfs_shared
LOCAL_SRC_FILES         := $(CRYSTAX_VFS_SRC_FILES) vfs/android_jni.cpp
LOCAL_C_INCLUDES        := $(CRYSTAX_INTERNAL_INCLUDES) $(LOCAL_PATH)/vfs $(LOCAL_PATH)/vfs/include
LOCAL_CFLAGS            := $(CRYSTAX_CFLAGS)
LOCAL_CPPFLAGS          := $(CRYSTAX_CPPFLAGS)
LOCAL_SHARED_LIBRARIES  := crystax_empty
LOCAL_LDLIBS            := $(CRYSTAX_LDLIBS)
LOCAL_EXPORT_CPPFLAGS   := -std=gnu++0x
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/vfs/include
LOCAL_EXPORT_LDLIBS     := $(CRYSTAX_LDLIBS)
include $(BUILD_SHARED_LIBRARY)

endif # CRYSTAX_VFS_FORCE_REBUILD == true
