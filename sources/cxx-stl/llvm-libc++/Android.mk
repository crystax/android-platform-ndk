# -*- makefile-gmake -*-
# This file is dual licensed under the MIT and the University of Illinois Open
# Source Licenses. See LICENSE.TXT for details.

LOCAL_PATH := $(call my-dir)

ifeq (,$(strip $(DEFAULT_LLVM_VERSION)))
$(error INTERNAL ERROR: 'DEFAULT_LLVM_VERSION' is empty!)
endif

__libcxx_version := $(strip \
    $(if $(filter clang%,$(NDK_TOOLCHAIN_VERSION)),\
        $(or \
            $(patsubst clang%,%,$(NDK_TOOLCHAIN_VERSION)),\
            $(DEFAULT_LLVM_VERSION)\
        ),\
        $(DEFAULT_LLVM_VERSION)\
    )\
)

# Temporary workaround - if using gcc, use libc++-3.6.
# This happens because using libc++-3.7 with gcc cause throwing of std::bad_cast,
# most likely due to incompatibility of used libc++abi with libc++-3.7.
# TODO: switch to use proper libc++abi, depending on libc++ versions.
# Here is ticket: https://tracker.crystax.net/issues/1177
ifeq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
__libcxx_version := 3.6
endif

ifeq (,$(strip $(__libcxx_version)))
$(error INTERNAL ERROR: Can not detect LLVM libc++ version!)
endif

# Normally, we distribute the NDK with prebuilt binaries of libc++
# in $LOCAL_PATH/<libcxx-version>/libs/<abi>/. However,
#

LIBCXX_FORCE_REBUILD := $(strip $(LIBCXX_FORCE_REBUILD))

__libcxx_force_rebuild := $(LIBCXX_FORCE_REBUILD)

ifndef LIBCXX_FORCE_REBUILD
  ifeq (,$(strip $(wildcard $(LOCAL_PATH)/$(__libcxx_version)/libs/$(TARGET_ARCH_ABI)/libc++_static$(TARGET_LIB_EXTENSION))))
    $(call __ndk_info,WARNING: Rebuilding libc++ libraries from sources!)
    $(call __ndk_info,You might want to use $$NDK/build/tools/build-cxx-stl.sh --stl=libc++)
    $(call __ndk_info,in order to build prebuilt versions to speed up your builds!)
    __libcxx_force_rebuild := true
  endif
endif

# Use gabi++ for x86* and mips* until libc++/libc++abi is ready for them
#ifneq (,$(filter x86% mips%,$(TARGET_ARCH_ABI)))
#  __prebuilt_libcxx_compiled_with_gabixx := true
#else
#  __prebuilt_libcxx_compiled_with_gabixx := false
#endif
__prebuilt_libcxx_compiled_with_gabixx := false

__libcxx_use_gabixx := $(__prebuilt_libcxx_compiled_with_gabixx)

LIBCXX_USE_GABIXX := $(strip $(LIBCXX_USE_GABIXX))
ifeq ($(LIBCXX_USE_GABIXX),true)
  __libcxx_use_gabixx := true
endif

ifneq ($(__libcxx_use_gabixx),$(__prebuilt_libcxx_compiled_with_gabixx))
  ifneq ($(__libcxx_force_rebuild),true)
    ifeq ($(__prebuilt_libcxx_compiled_with_gabixx),true)
      $(call __ndk_info,WARNING: Rebuilding libc++ libraries from sources since libc++ prebuilt libraries for $(TARGET_ARCH_ABI))
      $(call __ndk_info,are compiled with gabi++ but LIBCXX_USE_GABIXX is not set to true)
    else
      $(call __ndk_info,WARNING: Rebuilding libc++ libraries from sources since libc++ prebuilt libraries for $(TARGET_ARCH_ABI))
      $(call __ndk_info,are not compiled with gabi++ and LIBCXX_USE_GABIXX is set to true)
    endif
    __libcxx_force_rebuild := true
  endif
endif

llvm_libc++_includes := $(LOCAL_PATH)/$(__libcxx_version)/libcxx/include
llvm_libc++_export_includes := $(llvm_libc++_includes)
llvm_libc++_sources := \
	algorithm.cpp \
	bind.cpp \
	chrono.cpp \
	condition_variable.cpp \
	debug.cpp \
	exception.cpp \
	future.cpp \
	hash.cpp \
	ios.cpp \
	iostream.cpp \
	locale.cpp \
	memory.cpp \
	mutex.cpp \
	new.cpp \
	optional.cpp \
	random.cpp \
	regex.cpp \
	shared_mutex.cpp \
	stdexcept.cpp \
	string.cpp \
	strstream.cpp \
	system_error.cpp \
	thread.cpp \
	typeinfo.cpp \
	utility.cpp \
	valarray.cpp \

llvm_libc++_sources := $(llvm_libc++_sources:%=$(__libcxx_version)/libcxx/src/%)

# For now, this library can only be used to build C++11 binaries.
llvm_libc++_export_cxxflags := -std=c++11
llvm_libc++_export_cxxflags += -frtti -fexceptions

ifeq (,$(filter clang%,$(NDK_TOOLCHAIN_VERSION)))
# Add -fno-strict-aliasing because __list_imp::_end_ breaks TBAA rules by declaring
# simply as __list_node_base then casted to __list_node derived from that.  See
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61571 for details
llvm_libc++_export_cxxflags += -fno-strict-aliasing
endif

llvm_libc++_cxxflags := $(llvm_libc++_export_cxxflags)
llvm_libc++_cflags :=

ifeq ($(__libcxx_use_gabixx),true)

# Gabi++ emulates libcxxabi when building libcxx.
llvm_libc++_cxxflags += -DLIBCXXABI=1

# Find the GAbi++ sources to include them here.
# The voodoo below is to allow building libc++ out of the NDK source
# tree. This can make it easier to experiment / update / debug it.
#
libgabi++_sources_dir := $(strip $(wildcard $(LOCAL_PATH)/../gabi++))
ifdef libgabi++_sources_dir
  libgabi++_sources_prefix := ../gabi++
else
  libgabi++_sources_dir := $(strip $(wildcard $(NDK_ROOT)/sources/cxx-stl/gabi++))
  ifndef libgabi++_sources_dir
    $(error Can't find GAbi++ sources directory!!)
  endif
  libgabi++_sources_prefix := $(libgabi++_sources_dir)
endif

include $(libgabi++_sources_dir)/sources.mk
llvm_libc++_sources += $(addprefix $(libgabi++_sources_prefix:%/=%)/,$(libgabi++_src_files))
llvm_libc++_includes += $(libgabi++_c_includes)
llvm_libc++_export_includes += $(libgabi++_c_includes)

else
# libc++abi

libcxxabi_sources_dir := $(strip $(wildcard $(LOCAL_PATH)/../llvm-libc++abi))
ifdef libcxxabi_sources_dir
  libcxxabi_sources_prefix := ../llvm-libc++abi
else
  libcxxabi_sources_dir := $(strip $(wildcard $(NDK_ROOT)/sources/cxx-stl/llvm-libc++abi))
  ifndef libcxxabi_sources_dir
    $(error Can't find libcxxabi sources directory!!)
  endif
  libcxxabi_sources_prefix := $(libcxxabi_sources_dir)
endif

include $(libcxxabi_sources_dir)/sources.mk
llvm_libc++_sources += $(addprefix $(libcxxabi_sources_prefix:%/=%)/,$(libcxxabi_src_files))
llvm_libc++_includes += $(libcxxabi_c_includes)
llvm_libc++_export_includes += $(libcxxabi_c_includes)
# For armeabi*, use unwinder from libc++abi
ifneq (,$(filter armeabi%,$(TARGET_ARCH_ABI)))
llvm_libc++_cflags += -DLIBCXXABI_USE_LLVM_UNWINDER=1
llvm_libc++_sources += $(addprefix $(libcxxabi_sources_prefix:%/=%)/,$(libcxxabi_unwind_src_files))
else
llvm_libc++_cflags += -DLIBCXXABI_USE_LLVM_UNWINDER=0
endif

endif


ifneq ($(__libcxx_force_rebuild),true)

$(call ndk_log,Using prebuilt libc++ libraries)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_static
LOCAL_SRC_FILES := $(__libcxx_version)/libs/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE)$(TARGET_LIB_EXTENSION)
# For armeabi*, choose thumb mode unless LOCAL_ARM_MODE := arm
ifneq (,$(filter armeabi%,$(TARGET_ARCH_ABI)))
ifneq (arm,$(LOCAL_ARM_MODE))
ifneq (arm,$(TARGET_ARM_MODE))
LOCAL_SRC_FILES := $(__libcxx_version)/libs/$(TARGET_ARCH_ABI)/thumb/lib$(LOCAL_MODULE)$(TARGET_LIB_EXTENSION)
endif
endif
endif
LOCAL_EXPORT_C_INCLUDES := $(llvm_libc++_export_includes)
LOCAL_EXPORT_CPPFLAGS := $(llvm_libc++_export_cxxflags)
LOCAL_EXPORT_LDLIBS := -latomic
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_shared
LOCAL_SRC_FILES := $(__libcxx_version)/libs/$(TARGET_ARCH_ABI)/lib$(LOCAL_MODULE)$(TARGET_SONAME_EXTENSION)
# For armeabi*, choose thumb mode unless LOCAL_ARM_MODE := arm
ifneq (,$(filter armeabi%,$(TARGET_ARCH_ABI)))
ifneq (arm,$(LOCAL_ARM_MODE))
ifneq (arm,$(TARGET_ARM_MODE))
LOCAL_SRC_FILES := $(__libcxx_version)/libs/$(TARGET_ARCH_ABI)/thumb/lib$(LOCAL_MODULE)$(TARGET_SONAME_EXTENSION)
endif
endif
endif
LOCAL_EXPORT_C_INCLUDES := $(llvm_libc++_export_includes)
LOCAL_EXPORT_CPPFLAGS := $(llvm_libc++_export_cxxflags)
LOCAL_EXPORT_LDLIBS := -latomic
include $(PREBUILT_SHARED_LIBRARY)

else
# __libcxx_force_rebuild == true

$(call ndk_log,Rebuilding libc++ libraries from sources)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_static
LOCAL_SRC_FILES := $(llvm_libc++_sources)
LOCAL_C_INCLUDES := $(llvm_libc++_includes)
LOCAL_CFLAGS := $(llvm_libc++_cflags)
LOCAL_CPPFLAGS := $(llvm_libc++_cxxflags)
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_EXPORT_C_INCLUDES := $(llvm_libc++_export_includes)
LOCAL_EXPORT_CPPFLAGS := $(llvm_libc++_export_cxxflags)
LOCAL_LDLIBS := -latomic
LOCAL_EXPORT_LDLIBS := -latomic
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_shared
LOCAL_SRC_FILES := $(llvm_libc++_sources)
LOCAL_C_INCLUDES := $(llvm_libc++_includes)
LOCAL_CFLAGS := $(llvm_libc++_cflags)
LOCAL_CPPFLAGS := $(llvm_libc++_cxxflags)
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_EXPORT_C_INCLUDES := $(llvm_libc++_export_includes)
LOCAL_EXPORT_CPPFLAGS := $(llvm_libc++_export_cxxflags)
LOCAL_LDLIBS := -latomic
LOCAL_EXPORT_LDLIBS := -latomic
include $(BUILD_SHARED_LIBRARY)

endif # __libcxx_force_rebuild == true
