#
# Copyright (c) 2011-2015 CrystaX.
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
# THIS SOFTWARE IS PROVIDED BY CrystaX ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CrystaX OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of CrystaX.
#

MYDIR := $(dir $(firstword $(MAKEFILE_LIST)))

override MTARGET := crystax

ifeq (,$(strip $(NDK)))
NDK := $(realpath $(MYDIR)/../..)
endif

CCONF := $(MYDIR)/bin/config --ndk=$(NDK)

DEFAULT_GCC_VERSION := $(shell $(CCONF) --default-gcc-version)
ifeq (,$(DEFAULT_GCC_VERSION))
$(error Can not detect default GCC version!)
endif

ifeq (,$(strip $(TVS)))
TVS := gcc$(DEFAULT_GCC_VERSION)
endif

CCONF := $(CCONF) --toolchain=$(TVS)

MAKEPRG := $(MAKE) --no-print-directory

GENSOURCES := $(MYDIR)/bin/gen-sources

empty :=
space := $(empty) $(empty)
comma := ,

define commas-to-spaces
$(strip $(subst $(comma),$(space),$(1)))
endef

define spaces-to-commas
$(strip $(subst $(space),$(comma),$(strip $(1))))
endef

# $1: variable name where to save cached result
# $2: function name
# $3, $4, $5, ....: function arguments
# return: either cached result or result of function call (which is cached too)
define cached
$(strip $(if $(and $(strip $(1)),$(strip $(2))),\
    $(if $($(strip $(1))),,$(eval $(strip $(1)) := $(call $(strip $(2)),$(strip $(3)),$(strip $(4)),$(strip $(5)),$(strip $(6)),$(strip $(7)),$(strip $(8)),$(strip $(9)))))\
    $($(strip $(1))),\
    $(error Usage: call cached,var-name,func-name,func-arguments)\
))
endef

# $1: list
define head
$(firstword $(1))
endef

# $1: list
define tail
$(wordlist 2,$(words $(1)),$(1))
endef

define host-os-impl
$(shell uname -s | tr '[A-Z]' '[a-z]')
endef

define host-os
$(call cached,var-host-os,host-os-impl)
endef

define host-arch-impl
$(shell uname -m)
endef

define host-arch
$(call cached,var-host-arch,host-arch-impl)
endef

# $1: Toolchain version
define cctype
$(if $(filter clang%,$(1)),clang,gcc)
endef

# $1: Toolchain version (optional)
define is-gcc
$(if $(filter gcc,$(call cctype,$(or $(strip $(1)),$(TVS)))),yes)
endef

# $2: Toolchain version (optional)
define is-clang
$(if $(filter clang,$(call cctype,$(or $(strip $(1)),$(TVS)))),yes)
endef

# $1: ABI
define tcprefix-impl
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter armeabi%,$(1)),arm-linux-androideabi-),\
        $(if $(filter arm64-v8a,$(1)),aarch64-linux-android-),\
        $(if $(filter x86,$(1)),x86-),\
        $(if $(filter x86_64,$(1)),x86_64-),\
        $(if $(filter mips,$(1)),mipsel-linux-android-),\
        $(if $(filter mips64,$(1)),mips64el-linux-android-),\
        $(error Unsupported ABI: '$(1)')\
    ),\
    $(error Usage: call tcprefix,abi)\
))
endef

# $1: ABI
define tcprefix
$(call cached,var-tcprefix-$(1),tcprefix-impl,$(1))
endef

# $1: ABI
define tcname-impl
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter x86,$(1)),i686-linux-android-),\
        $(if $(filter x86_64,$(1)),x86_64-linux-android-),\
        $(call tcprefix,$(1))\
    ),\
    $(error Usage: call tcname,abi)\
))
endef

# $1: ABI
define tcname
$(call cached,var-tcname-$(1),tcname-impl,$(1))
endef

# $1: ABI
define llvm-tripple-impl
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter armeabi,$(1)),armv5te-none-linux-androideabi),\
        $(if $(filter armeabi-v7a%,$(1)),armv7-none-linux-androideabi),\
        $(if $(filter arm64-v8a,$(1)),aarch64-none-linux-android),\
        $(if $(filter x86,$(1)),i686-none-linux-android),\
        $(if $(filter x86_64,$(1)),x86_64-none-linux-android),\
        $(if $(filter mips,$(1)),mipsel-none-linux-android),\
        $(if $(filter mips64,$(1)),mips64el-none-linux-android),\
        $(error Unsupported ABI: '$(1)')\
    ),\
    $(error Usage: call llvm-tripple,abi)\
))
endef

# $1: ABI
define llvm-tripple
$(call cached,var-llvm-tripple-$(1),llvm-tripple-impl,$(1))
endef

# $1: ABI
# $2: GCC version
define gcc-toolchain-impl
$(abspath $(NDK)/toolchains/$(call tcprefix,$(1))$(2)/prebuilt/$(host-os)-$(host-arch))
endef

# $1: ABI
# $2: GCC version
define gcc-toolchain
$(call cached,var-gcc-toolchain-$(1)-$(2),gcc-toolchain-impl,$(1),$(2))
endef

# $1: Toolchain version
define gcc-version
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter gcc%,$(1)),$(patsubst gcc%,%,$(1))),\
        $(error '$(1)' is not gcc)\
    ),\
    $(error Usage: call gcc-version,toolchain-version)\
))
endef

# $1: Toolchain version
define clang-version
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter clang%,$(1)),$(patsubst clang%,%,$(1))),\
        $(error '$(1)' is not clang)\
    ),\
    $(error Usage: call clang-version,toolchain-version)\
))
endef

# $1: ABI
# $2: Toolchain version
# $3: Toolchain utility name (gcc, clang, ar etc)
define tc-bin-impl
$(strip $(if $(and $(strip $(1)),$(strip $(2)),$(strip $(3))),\
    $(if $(call is-gcc,$(2)),\
        $(call gcc-toolchain,$(1),$(call gcc-version,$(2)))/bin/$(call tcname,$(1))$(strip $(3)),\
        $(strip \
            $(abspath $(NDK)/toolchains/llvm-$(call clang-version,$(2))/prebuilt/$(host-os)-$(host-arch)/bin/$(strip $(3)))\
            $(if $(filter clang clang++,$(3)),\
                -target $(call llvm-tripple,$(1))\
                -gcc-toolchain $(call gcc-toolchain,$(1),$(DEFAULT_GCC_VERSION))\
            )\
        )\
    ),\
    $(error Usage: call tc-bin,abi,toolchain-version,name)\
))
endef

# $1: ABI
# $2: Toolchain version
# $3: Toolchain utility name (gcc, clang, ar etc)
define tc-bin
$(call cached,var-tc-bin-$(1)-$(2)-$(3),tc-bin-impl,$(1),$(2),$(3))
endef

# $1: ABI
# $2: Toolchain version (optional)
define cc
$(call tc-bin,$(1),$(or $(strip $(2)),$(TVS)),$(if $(call is-gcc,$(or $(strip $(2)),$(TVS))),gcc,clang))
endef

# $1: ABI
# $2: Toolchain version (optional)
define c++
$(call tc-bin,$(1),$(or $(strip $(2)),$(TVS)),$(if $(call is-gcc,$(or $(strip $(2)),$(TVS))),g++,clang++))
endef

# $1: ABI
# $2: Toolchain version (optional)
define ar
$(call tc-bin,$(1),$(or $(strip $(2)),$(TVS)),$(if $(call is-gcc,$(or $(strip $(2)),$(TVS))),ar,llvm-ar))
endef

# $1: ABI
define arch-for-abi-impl
$(strip $(if $(filter 1,$(words $(1))),\
    $(or \
        $(if $(filter armeabi%,$(1)),arm),\
        $(if $(filter arm64%,$(1)),arm64),\
        $(if $(filter x86 x86_64 mips mips64,$(1)),$(1)),\
        $(error Unsupported ABI: '$(1)')\
    ),\
    $(error Usage: call arch-for-abi,abi)\
))
endef

# $1: ABI
define arch-for-abi
$(call cached,var-arch-for-abi-$(1),arch-for-abi-impl,$(1))
endef

# $1: ABI
# $2: list of API levels
define detect-platform-impl
$(strip $(if $(filter 1,$(words $(1))),\
    $(if $(strip $(2)),\
        $(if $(wildcard $(NDK)/platforms/android-$(call head,$(2))/arch-$(call arch-for-abi,$(1))),\
            android-$(call head,$(2)),\
            $(call detect-platform-impl,$(1),$(call tail,$(2)))\
        ),\
        $(error Can not detect sysroot platform for ABI '$(1)')\
    ),\
    $(error Usage: call detect-platform,abi,api-levels)\
))
endef

# $1: ABI
# $2: list of API levels
define detect-platform
$(call cached,var-detect-platform-$(1),detect-platform-impl,$(1),$(2))
endef

# $1: ABI
define sysroot
$(strip $(if $(filter 1,$(words $(1))),\
    $(abspath $(NDK)/platforms/$(call detect-platform,$(1),9 L 21)/arch-$(call arch-for-abi,$(1))),\
    $(error Usage: call sysroot,abi)\
))
endef

# $1: ABI
# $2: multilib variant
define sysroot-libdir
$(strip $(if $(and $(filter 1,$(words $(1))),$(filter 1,$(words $(2)))),\
    $(call sysroot,$(1))/usr/$(strip \
        $(or \
            $(if $(filter x32,$(2)),libx32),\
            $(if $(filter mips32r2,$(2)),libr2),\
            $(if $(filter mips32r6,$(2)),libr6),\
            $(if $(filter mips64r2,$(2)),lib64r2),\
            $(if $(filter x86_64 mips64r6,$(2)),lib64),\
            lib\
        )\
    ),\
    $(error Usage: call sysroot-libdir,abi,multilib)\
))
endef

define abis-impl
$(or $(call commas-to-spaces,$(ABIS)),$(call commas-to-spaces,$(ABI)),$(shell $(CCONF) --abis))
endef

define abis
$(call cached,var-abis,abis-impl)
endef

# $1: ABI
define multilibs
$(shell $(CCONF) --multilibs --abi=$(1))
endef

# $1: type (shared or static)
define targetname
$(strip \
    $(if $(strip $(1)),,$(error Usage: call targetname,type))\
    $(if $(filter 1,$(words $(1))),,$(error More than one parameters passed to targetname function))\
    libcrystax.$(subst shared,so,$(subst static,a,$(1)))\
)
endef

# $1: type (shared or static)
# $2: ABI
# $3: multilib variant (optional)
define target
$(abspath $(MYDIR)/$(shell $(CCONF) --libpath --abi=$(strip $(2)) $(if $(strip $(3)),--multilib=$(strip $(3))))/$(call targetname,$(1)))
endef

define static-libraries
endef

# $1: name
# $2: ABI
# $3: multilib variant
define static-library
$(call objdir,$(2),$(3))/lib$(1).a
endef

define stubs
dl bionic
endef

# $1: stub name
# $2: ABI
# $3: multilib variant (optional)
define stub
$(abspath $(MYDIR)/$(shell $(CCONF) --libpath --abi=$(strip $(2)) $(if $(strip $(3)),--multilib=$(strip $(3))))/stubs/lib$(1)stub.so)
endef

# $1: stub name
define stub-symbols
$(strip $(or \
    $(if $(filter dl,$(1)),\
        dl_iterate_phdr      \
        dl_unwind_find_exidx \
        dladdr               \
        dlclose              \
        dlerror              \
        dlopen               \
        dlsym                \
    ),\
    $(if $(filter bionic,$(1)),\
        __crystax_bionic_stub \
    ),\
    $(error Unknown stub: '$(1)')\
))
endef

# $1: stub name
define stub-soname
$(if $(filter bionic,$(1)),libc.so,lib$(1).so)
endef

# $1: library name
# $2: type (shared or static)
# $3: ABI
# $4: multilib variant (optional)
define sources-impl
$(strip $(or \
    $(if $(filter $(MTARGET),$(1)),\
        $(shell $(CCONF) --sources --abi=$(strip $(3)) --libtype=$(strip $(2)) \
            $(if $(strip $(4)),--multilib=$(strip $(4)))) \
    ),\
    $(error Unknown library: '$(1)')\
))
endef

# $1: library name
# $2: type (shared or static)
# $3: ABI
# $4: multilib variant (optional)
define sources
$(call cached,var-sources-$(1)-$(2)-$(3)-$(4),sources-impl,$(1),$(2),$(3),$(4))
endef

# $1: library name
# $2: ABI
# $3: multilib variant (optional)
# $4: source file (optional)
define cflags
$(strip $(or \
    $(if $(filter $(MTARGET),$(1)),\
        $(shell $(strip $(CCONF) --cflags --abi=$(strip $(2)) \
            $(if $(strip $(3)),--multilib=$(strip $(3))) \
            $(if $(strip $(4)),--source-file=$(strip $(4))) \
        ))\
    ),\
    $(call abiflags,$(2),$(3))\
))
endef

# $1: library name
# $2: ABI
# $3: multilib variant (optional)
# $4: source file (optional)
define c++flags
$(strip $(or \
    $(if $(filter $(MTARGET),$(1)),\
        $(shell $(CCONF) --c++flags --abi=$(strip $(2)) \
            $(if $(strip $(3)),--multilib=$(strip $(3))) \
            $(if $(strip $(4)),--source-file=$(strip $(4))) \
        )\
    ),\
    $(call abiflags,$(2),$(3))\
))
endef

# $1: library name
# $2: ABI
# $3: multilib variant (optional)
# $4: source file (optional)
define asmflags
$(strip $(or \
    $(if $(filter $(MTARGET),$(1)),\
        $(shell $(CCONF) --asmflags --abi=$(strip $(2)) \
            $(if $(strip $(3)),--multilib=$(strip $(3))) \
            $(if $(strip $(4)),--source-file=$(strip $(4))) \
        )\
    ),\
    $(call abiflags,$(2),$(3))\
))
endef

# $1: ABI
# $2: multilib variant
define abiflags-impl
$(shell $(CCONF) --abiflags --abi=$(strip $(1)) --multilib=$(strip $(2)))
endef

# $1: ABI
# $2: multilib variant
define abiflags
$(call cached,var-abiflags-$(1)-$(2),abiflags-impl,$(1),$(2))
endef

# $1: ABI
# $2: multilib variant (optional)
define ldflags-impl
$(shell $(CCONF) --ldflags --abi=$(strip $(1)) $(if $(strip $(2)),--multilib=$(strip $(2))))
endef

# $1: ABI
# $2: multilib variant (optional)
define ldflags
$(call cached,var-ldflags-$(1)-$(2),ldflags-impl,$(1),$(2))
endef

define objroot
$(or $(strip $(OBJDIR)),$(abspath $(MYDIR)/obj))
endef

# $1: ABI
# $2: multilib variant
# $3: library name (optional)
define objdir
$(strip $(if $(and $(strip $(1)),$(strip $(2))),\
    $(objroot)/$(strip $(or \
        $(if $(filter armeabi,$(1)),armv5te$(if $(filter arm,$(2)),,-$(2))),\
        $(if $(filter armeabi-v7a,$(1)),armv7-a$(if $(filter arm,$(2)),,-$(2))),\
        $(if $(filter armeabi-v7a-hard,$(1)),armv7-a$(if $(filter arm,$(2)),,-$(2))-hard),\
        $(if $(filter arm64-v8a x86 x86_64 mips mips64,$(1)),$(2)),\
        $(error Unsupported ABI: '$(1)')\
    ))$(if $(strip $(3)),/$(strip $(3))),\
    $(error Usage: call objdir,abi,multilib,libname)\
))
endef

# $1: library name
# $2: type (shared or static)
# $3: ABI
# $4: multilib variant
define objfiles-impl
$(strip \
    $(addprefix $(call objdir,$(3),$(4),$(1))/,\
        $(foreach __f,$(call sources,$(1),$(2),$(3),$(4)),$(call objfile,$(__f)))\
    )\
)
endef

# $1: library name
# $2: type (shared or static)
# $3: ABI
# $4: multilib variant
define objfiles
$(call cached,var-objfiles-$(1)-$(2)-$(3)-$(4),objfiles-impl,$(1),$(2),$(3),$(4))
endef

# $1: source file
define objfile
$(addsuffix .o,$(subst $(abspath $(MYDIR))/,,$(abspath $(1))))
endef

# $1: library name
# $2: ABI
# $3: multilib variant
# $4: source file
define full-objfile-path
$(call objdir,$(2),$(3),$(1))/$(call objfile,$(4))
endef

# $1: source file
define source-file-language
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter %.c,$(1)),C),\
        $(if $(filter %.cpp %.cc,$(1)),C++),\
        $(if $(filter %.s %.S,$(1)),ASM),\
        $(error Can not detect language of source file '$(1)')\
    ),\
    $(error Usage: call source-file-language,path)\
))
endef

# $1: ABI
# $2: source file
define compiler-for
$(strip $(if $(and $(strip $(1)),$(strip $(2))),\
    $(or \
        $(if $(filter C ASM,$(call source-file-language,$(2))),$(call cc,$(1))),\
        $(if $(filter C++,$(call source-file-language,$(2))),$(call c++,$(1))),\
        $(error Unknown source file language for '$(2)': '$(call source-file-language,$(2))')\
    ),\
    $(error Usage: call compiler-for,source-file)\
))
endef

# $1: library name
# $2: ABI
# $3: multilib variant
# $4: source file
define compiler-flags
$(strip $(or \
    $(if $(filter C,  $(call source-file-language,$(4))),$(call cflags,$(1),$(2),$(3),$(4))),\
    $(if $(filter C++,$(call source-file-language,$(4))),$(call c++flags,$(1),$(2),$(3),$(4))),\
    $(if $(filter ASM,$(call source-file-language,$(4))),$(call asmflags,$(1),$(2),$(3),$(4))),\
    $(error Could not detect compiler flags for '$(4)')\
))
endef

# $1: Toolchain version (optional)
define ar-flags
crs$(if $(call is-gcc,$(1)),D)
endef

# $1: ABI
# $2: target library file
define linker-options
-Wl,-shared -Wl,-soname,$(notdir $(2)) -nostdlib --sysroot=$(call sysroot,$(1))
endef

# $1: ABI
# $2: multilib variant
# $3: file name
define linker-crtfile
$(strip $(if $(filter 1,$(words $(3))),\
    $(call sysroot-libdir,$(1),$(2))/$(strip $(3)),\
    $(error Usage: call linker-crtfile,abi,multilib,file-name)\
))
endef

# $1: root directory
# $2: wildcards (*.c, *.h etc)
define rwildcard
$(foreach __d,$(wildcard $(1)*),$(call rwildcard,$(__d)/,$(2)) $(filter $(subst *,%,$(2)),$(__d)))
endef

define hide
$(if $(filter 1,$(V)),,@)
endef

define rm-if-exists
$(if $(wildcard $(1)),rm -Rf $(1))
endef

define auto-generated-sources
$(shell sed -n '/# BEGIN OF AUTO-GENERATED SOURCES/,/# END OF AUTO-GENERATED SOURCES/p' $(MYDIR)/.gitignore | grep -v '^#')
endef

define additional-dependencies
$(filter-out %.d,$(MAKEFILE_LIST)) $(firstword $(CCONF))
endef

# $1: library name
# $2: ABI
# $3: multilib variant
# $4: source file
define add-objfile-rule
$$(call full-objfile-path,$(1),$(2),$(3),$(4)): $$(abspath $(4)) $$(additional-dependencies)
	$$(eval __lnam := $(1))
	$$(eval __abi  := $(2))
	$$(eval __mlib := $(3))
	$$(eval __srcf := $(4))
	@echo "CC $$(__abi) [$$(__mlib)] $$(__srcf)"
	@mkdir -p $$(dir $$@)
	$$(hide)$$(call compiler-for,$$(__abi),$$<) \
		-MD -MP -MF $$(patsubst %.o,%.d,$$@) \
		$$(call compiler-flags,$$(__lnam),$$(__abi),$$(__mlib),$$(__srcf)) \
		--sysroot=$$(call sysroot,$$(__abi)) \
		-c -o $$@ $$<
endef

# $1: type (shared or static)
# $2: ABI
# $3: multilib variant
define add-target-rule
$$(call target,$(1),$(2),$(3)): $$(strip \
		$$(call objfiles,$$(MTARGET),$(1),$(2),$(3)) \
		$$(foreach __static_lib,$$(call static-libraries),\
			$$(call static-library,$$(__static_lib),$(2),$(3)) \
		)\
		$$(if $$(filter shared,$(1)),\
			$$(foreach __stub,$$(call stubs),\
				$$(call stub,$$(__stub),$(2),$(3)) \
			)\
		)\
		$$(additional-dependencies))
	$$(eval __type := $(1))
	$$(eval __abi  := $(2))
	$$(eval __mlib := $(3))
	$$(eval __objfiles := $$(call objfiles,$$(MTARGET),$$(__type),$$(__abi),$$(__mlib)))
	$$(eval __crtbegin_objfile := $$(filter %/crtbegin_so.o,$$(__objfiles)))
	$$(eval __crtbrand_objfile := $$(filter %/crtbrand.o,$$(__objfiles)))
	$$(eval __crtend_objfile   := $$(filter %/crtend_so.o,$$(__objfiles)))
	$$(eval __common_objfiles := $$(filter-out $$(addprefix %/,crtbegin_so.o crtbrand.o crtend_so.o),$$(__objfiles)))
	$$(foreach __static_lib,$$(call static-libraries),\
		$$(eval __common_objfiles += $$(call objfiles,$$(__static_lib),static,$$(__abi),$$(__mlib)))\
	)
	@echo "$$(if $$(filter static,$$(__type)),AR,LD) $$(__abi) [$$(__mlib)] $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	@rm -f $$@
	$$(hide)$$(strip $$(if $$(filter static,$$(__type)),\
		$$(call ar,$$(__abi)) \
			$$(call ar-flags) \
			$$@ \
			$$(__common_objfiles) \
			,\
		$$(call c++,$$(__abi)) \
			$$(call abiflags,$$(__abi),$$(__mlib)) \
			$$(call linker-options,$$(__abi),$$@) \
			$$(__crtbegin_objfile) \
			$$(__crtbrand_objfile) \
			$$(__common_objfiles) \
			$$(__crtend_objfile) \
			$$(call ldflags,$$(__abi),$$(__mlib)) \
			$$(foreach __stub,$$(call stubs),\
				-L$$(dir $$(call stub,$$(__stub),$(2),$(3))) \
				-Wl,-Bdynamic,-l$$(patsubst lib%.so,%,$$(notdir $$(call stub,$$(__stub),$(2),$(3)))) \
			)\
			-o $$@ \
	))

.PHONY: all
all: $$(call target,$(1),$(2),$(3))

.PHONY: $(1)-$(2)
$(1)-$(2): $$(call target,$(1),$(2),$(3))

endef

# $1: library name
# $2: ABI
# $3: multilib variant
define add-static-library-rule
$$(call static-library,$(1),$(2),$(3)): $$(call objfiles,$(1),static,$(2),$(3)) $$(additional-dependencies)
	$$(eval __name := $(1))
	$$(eval __abi  := $(2))
	$$(eval __mlib := $(3))
	@echo "AR $$(__abi) [$$(__mlib)] $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	@rm -f $$@
	$$(hide)$$(call ar,$$(__abi)) $$(call ar-flags) $$@ \
		$$(call objfiles,$$(__name),static,$$(__abi),$$(__mlib))
endef

# $1: stub name
# $2: ABI
# $3: multilib variant
define add-stub-rule

$$(call stub,$(1),$(2),$(3)): $$(call objdir,$(2),$(3),$(1)stub)/stub.c
	@echo "ST $(2) [$(3)] $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	$$(hide)$$(call cc,$(2)) \
		$$(call abiflags,$(2),$(3)) \
		-Wl,-shared,-Bsymbolic \
		-Wl,-soname,$$(call stub-soname,$(1)) \
		-nostdlib \
		-o $$@ $$< \
		-Wl,--exclude-libs,libgcc.a

$$(call objdir,$(2),$(3),$(1)stub)/stub.c: $$(additional-dependencies)
	@mkdir -p $$(dir $$@)
	@rm -f $$@
	@$$(foreach __sym,$$(call stub-symbols,$(1)),\
		echo "void $$(__sym)() {}" >>$$@ || exit 1; \
	)

endef

# $1: type (static or shared)
define add-type-build-rule
.PHONY: $(1)
$(1): gen-sources
	@+$$(foreach __abi,$$(call abis),\
		$$(MAKE) -C $$(realpath $$(MYDIR)) $(1)-$$(__abi) CRYSTAX_EVAL_RULES=yes ABI=$$(__abi) || exit 1; \
	)
endef

define add-all-build-rule
.PHONY: all
all: gen-sources
	@+$$(foreach __abi,$$(call abis),\
		$$(MAKE) -C $$(realpath $$(MYDIR)) $$(foreach __t,static shared,$$(__t)-$$(__abi)) CRYSTAX_EVAL_RULES=yes ABI=$$(__abi) || exit 1; \
	)
endef

#=====================================================================================================================

.PHONY: all
all:

.PHONY: clean
clean:
	$(call rm-if-exists,$(strip \
		$(abspath $(MYDIR)/libs) \
		$(objroot) \
		$(addprefix $(abspath $(MYDIR))/,$(auto-generated-sources)) \
	))

.PHONY: rebuild
rebuild:
	@$(MAKE) clean
	@$(MAKE) all

.PHONY: gen-sources
gen-sources:
	@$(GENSOURCES)

ifeq (yes,$(CRYSTAX_EVAL_RULES))
$(foreach __abi,$(call abis),\
    $(foreach __mlib,$(call multilibs,$(__abi)),\
        $(foreach __stub,$(call stubs),\
            $(eval $(call add-stub-rule,$(__stub),$(__abi),$(__mlib)))\
        )\
        $(foreach __libname,$(call static-libraries),\
            $(eval $(call add-static-library-rule,$(__libname),$(__abi),$(__mlib)))\
            $(foreach __src,$(call sources,$(__libname),static,$(__abi),$(__mlib)),\
                $(eval $(call add-objfile-rule,$(__libname),$(__abi),$(__mlib),$(__src)))\
            )\
        )\
        $(foreach __src,\
            $(sort $(foreach __t,static shared,\
                $(call sources,$(MTARGET),$(__t),$(__abi),$(__mlib))\
            )),\
            $(eval $(call add-objfile-rule,$(MTARGET),$(__abi),$(__mlib),$(__src)))\
        )\
        $(foreach __t,static shared,\
            $(eval $(call add-target-rule,$(__t),$(__abi),$(__mlib)))\
        )\
        $(eval sinclude $(call rwildcard,$(call objdir,$(__abi),$(__mlib)),*.d))\
    )\
)
else
$(eval $(call add-all-build-rule))
$(foreach __t,static shared,\
    $(eval $(call add-type-build-rule,$(__t)))\
)
endif
