MYDIR := $(dir $(firstword $(MAKEFILE_LIST)))

ifeq (,$(strip $(NDK)))
NDK := $(realpath $(MYDIR)/../..)
endif

ifeq (,$(strip $(TVS)))
TVS := gcc4.9
endif

MAKEPRG := $(MAKE) --no-print-directory

GENSOURCES := $(MYDIR)/bin/gen-sources

CCONF := $(MYDIR)/bin/config --ndk=$(NDK) --toolchain=$(TVS)

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

# $1: Toolchain version
define is-gcc
$(if $(filter gcc,$(call cctype,$(1))),yes)
endef

# $2: Toolchain version
define is-clang
$(if $(filter clang,$(call cctype,$(1))),yes)
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
                -gcc-toolchain $(call gcc-toolchain,$(1),4.9)\
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
# $2: Toolchain version
define cc
$(call tc-bin,$(1),$(2),$(if $(call is-gcc,$(2)),gcc,clang))
endef

# $1: ABI
# $2: Toolchain version
define c++
$(call tc-bin,$(1),$(2),$(if $(call is-gcc,$(2)),g++,clang++))
endef

# $1: ABI
# $2: Toolchain version
define ar
$(call tc-bin,$(1),$(2),$(if $(call is-gcc,$(2)),ar,llvm-ar))
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

# $1: ABI
define sources-impl
$(shell $(CCONF) --sources --abi=$(strip $(1)))
endef

# $1: ABI
define sources
$(call cached,var-sources-$(1),sources-impl,$(1))
endef

# $1: ABI
# $2: multilib variant (optional)
define cflags-impl
$(shell $(CCONF) --cflags --abi=$(strip $(1)) $(if $(strip $(2)),--multilib=$(strip $(2))))
endef

# $1: ABI
# $2: multilib variant (optional)
define cflags
$(call cached,var-cflags-$(1)-$(2),cflags-impl,$(1),$(2))
endef

# $1: ABI
# $2: multilib variant (optional)
define c++flags-impl
$(shell $(CCONF) --c++flags --abi=$(strip $(1)) $(if $(strip $(2)),--multilib=$(strip $(2))))
endef

# $1: ABI
# $2: multilib variant (optional)
define c++flags
$(call cached,var-c++flags-$(1)-$(2),c++flags-impl,$(1),$(2))
endef

# $1: ABI
# $2: multilib variant (optional)
define asmflags-impl
$(shell $(CCONF) --asmflags --abi=$(strip $(1)) $(if $(strip $(2)),--multilib=$(strip $(2))))
endef

# $1: ABI
# $2: multilib variant (optional)
define asmflags
$(call cached,var-asmflags-$(1)-$(2),asmflags-impl,$(1),$(2))
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
define objdir
$(strip $(if $(and $(strip $(1)),$(strip $(2))),\
    $(objroot)/$(strip $(1))-$(strip $(2)),\
    $(error Usage: call objdir,abi,multilib)\
))
endef

# $1: type (shared or static)
# $2: ABI
# $3: multilib variant
define objfiles-impl
$(strip \
    $(addprefix $(call objdir,$(2),$(3))/,\
        $(foreach __f,$(call sources,$(2)),$(call objfile,$(__f)))\
    )\
)
endef

# $1: type (shared or static)
# $2: ABI
# $3: multilib variant
define objfiles
$(call cached,var-objfiles-$(1)-$(2)-$(3),objfiles-impl,$(1),$(2),$(3))
endef

# $1: source file
define objfile
$(addsuffix .o,$(subst $(abspath $(MYDIR))/,,$(abspath $(1))))
endef

# $1: source file
define source-file-language
$(strip $(if $(strip $(1)),\
    $(or \
        $(if $(filter %.c,$(1)),C),\
        $(if $(filter %.cpp %.cc,$(1)),C++),\
        $(if $(filter %.s,$(1)),ASM),\
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
        $(if $(filter C ASM,$(call source-file-language,$(2))),$(call cc,$(1),$(TVS))),\
        $(if $(filter C++,$(call source-file-language,$(2))),$(call c++,$(1),$(TVS))),\
        $(error Unknown source file language for '$(2)': '$(call source-file-language,$(2))')\
    ),\
    $(error Usage: call compiler-for,source-file)\
))
endef

# $1: ABI
# $2: multilib variant
# $3: source file
define compiler-flags
$(strip $(or \
    $(if $(filter C,  $(call source-file-language,$(3))),$(call cflags,$(1),$(2))),\
    $(if $(filter C++,$(call source-file-language,$(3))),$(call c++flags,$(1),$(2))),\
    $(if $(filter ASM,$(call source-file-language,$(3))),$(call asmflags,$(1),$(2))),\
    $(error Could not detect compiler flags for '$(3)')\
))
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
#$(foreach __d,$(wildcard $(1)*),$(call rwildcard,$(__d)/,$(2)) $(filter $(subst *,%,$(2)),$(__d)))
define rwildcard
$(foreach __mask,$(2),$(shell find $(1) -name '$(2)' -print))
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

# $1: ABI
# $2: multilib variant
# $3: source file
define add-objfile-rule
$$(call objdir,$(1),$(2))/$$(call objfile,$(3)): $$(abspath $(3)) $$(MAKEFILE_LIST) $$(firstword $$(CCONF))
	@echo "CC $(1) [$(2)] $(3)"
	@mkdir -p $$(dir $$@)
	$$(hide)$$(call compiler-for,$(1),$$<) \
		-MD -MP -MF $$(patsubst %.o,%.d,$$@) \
		$$(call compiler-flags,$(1),$(2),$(3)) \
		--sysroot=$$(call sysroot,$(1)) \
		-c -o $$@ $$<
endef

# $1: type (shared or static)
# $2: ABI
# $3: multilib variant
define add-target-rule
$$(call target,$(1),$(2),$(3)): $$(call objfiles,$(1),$(2),$(3)) $$(MAKEFILE_LIST) $$(firstword $$(CCONF))
	@echo "$(if $(filter static,$(1)),AR,LD) $(2) [$(3)] $$(notdir $$@)"
	@mkdir -p $$(dir $$@)
	@rm -f $$@
	$$(hide)$(strip $(if $(filter static,$(1)),\
		$$(call ar,$(2),$$(TVS)) crs$$(if $$(call is-gcc,$$(TVS)),D) $$@ \
			$$(filter-out $$(MAKEFILE_LIST),$$^),\
		$$(call c++,$(2),$$(TVS)) $$(call linker-options,$(2),$$@) \
			-L$$(call sysroot-libdir,$(2),$(3)) \
			$$(call linker-crtfile,$(2),$(3),crtbegin_so.o) \
			$$(call objfiles,$(1),$(2),$(3)) \
			-lgcc \
			$$(call ldflags,$(2),$(3)) \
			$$(call linker-crtfile,$(2),$(3),crtend_so.o) \
			-o $$@ \
	))

.PHONY: all
all: $$(call target,$(1),$(2),$(3))

.PHONY: $(1)-$(2)
$(1)-$(2): $$(call target,$(1),$(2),$(3))

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
	$(call rm-if-exists,$(abspath $(MYDIR)/libs))
	$(call rm-if-exists,$(objroot))
	$(call rm-if-exists,$(addprefix $(abspath $(MYDIR))/,$(auto-generated-sources)))

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
        $(foreach __t,static shared,\
            $(eval $(call add-target-rule,$(__t),$(__abi),$(__mlib)))\
        )\
        $(foreach __src,$(call sources,$(__abi)),\
            $(eval $(call add-objfile-rule,$(__abi),$(__mlib),$(__src)))\
        )\
        $(if $(wildcard $(call objdir,$(__abi),$(__mlib))),\
            $(eval include $(call rwildcard,$(call objdir,$(__abi),$(__mlib),*.d)))\
        )\
    )\
)
else
$(eval $(call add-all-build-rule))
$(foreach __t,static shared,\
    $(eval $(call add-type-build-rule,$(__t)))\
)
endif
