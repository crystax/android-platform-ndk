ifneq (yes,$(shell which grep >/dev/null 2>&1 && echo yes))
$(error No 'grep' tool found)
endif

ifneq (yes,$(shell which awk >/dev/null 2>&1 && echo yes))
$(error No 'awk' tool found)
endif

downcase = $(strip \
$(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,\
$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,\
$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,\
$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(1))))))))))))))))))))))))))))

define is-true
$(if $(filter 1 yes true on,$(call downcase,$(1))),yes)
endef

define preprocess
$(strip $(if $(and $(strip $(1)),$(strip $(2))),\
    $(shell echo $(2) | $(1) -x c -E - | tail -n 1),\
    $(error Usage: call preprocess,compiler,string)\
))
endef

define compiler-type
$(strip $(if $(strip $(1)),\
    $(if $(filter-out __GNUC__,$(call preprocess,$(1),__GNUC__)),\
        $(if $(filter __clang__,$(call preprocess,$(1),__clang__)),gcc,clang),\
        $(error Cannot detect type of compiler '$(1)')\
    ),\
    $(error Usage: call compiler-type,compiler)\
))
endef

define is-gcc
$(if $(and $(strip $(1)),$(filter gcc,$(call compiler-type,$(1)))),yes)
endef

define is-clang
$(if $(and $(strip $(1)),$(filter clang,$(call compiler-type,$(1)))),yes)
endef

define gcc-major-version
$(strip $(if $(call is-gcc,$(1)),\
    $(call preprocess,$(1),__GNUC__)\
))
endef

define gcc-minor-version
$(strip $(if $(call is-gcc,$(1)),\
    $(call preprocess,$(1),__GNUC_MINOR__)\
))
endef

define gcc-version
$(strip $(if $(strip $(1)),\
    $(if $(call is-gcc,$(1)),$(call gcc-major-version,$(1)).$(call gcc-minor-version,$(1))),\
    $(error Usage: call gcc-version,cc)\
))
endef

define clang-major-version
$(strip $(if $(call is-clang,$(1)),\
    $(call preprocess,$(1),__clang_major__)\
))
endef

define clang-minor-version
$(strip $(if $(call is-clang,$(1)),\
    $(call preprocess,$(1),__clang_minor__)\
))
endef

define c++11
$(strip \
    $(if \
        $(and \
            $(call is-gcc,$(CC)),\
            $(filter 4,$(call gcc-major-version,$(CC))),\
            $(filter 6,$(call gcc-minor-version,$(CC)))\
        ),\
        c++0x,\
        c++11\
    )\
)
endef

define host-os
$(shell uname -s | tr '[A-Z]' '[a-z]')
endef

define is-host-os-linux
$(if $(filter linux,$(call host-os)),yes)
endef

define is-host-os-darwin
$(if $(filter darwin,$(call host-os)),yes)
endef

define head
$(firstword $(1))
endef

ifneq (a,$(call head,a b c))
$(error Function 'head' broken)
endif

define tail
$(wordlist 2,$(words $(1)),$(1))
endef

ifneq (b c,$(call tail,a b c))
$(error Function 'tail' broken)
endif

define objc-runtime
$(if $(or $(and $(call is-clang,$(CC)),$(call is-host-os-darwin)),$(call is-old-apple-gcc,$(CC))),next,gnu)
endef

c-sources-masks = %.c
c++-sources-masks = %.cpp %.cc
objective-c-sources-masks = %.m
objective-c++-sources-masks = %.mm

all-sources-masks = $(c-sources-masks) $(c++-sources-masks) $(objective-c-sources-masks) $(objective-c++-sources-masks)

define has-c-sources
$(if $(filter $(c-sources-masks) $(objective-c-sources-masks),$(SRCFILES)),yes)
endef

define has-objective-c-sources
$(if $(filter $(objective-c-sources-masks) $(objective-c++-sources-masks),$(SRCFILES)),yes)
endef

define has-c++-sources
$(if $(filter $(c++-sources-masks) $(objective-c++-sources-masks),$(SRCFILES)),yes)
endef

# $1: regexp
# $2: string
define match
$(shell echo $(2) | grep -iq $(1) && echo yes)
endef

define is-old-macosx
$(strip $(and \
    $(call is-host-os-darwin),\
    $(shell test $$(sw_vers -productVersion | awk -F. '{print $$1 * 10000 + $$2 * 100 + $$3}') -lt 100900 && echo yes)\
))
endef

define is-old-apple-gcc
$(strip $(and \
    $(call is-host-os-darwin),\
    $(call is-gcc,$(1)),\
    $(filter 4,$(call gcc-major-version,$(1))),\
    $(filter 2,$(call gcc-minor-version,$(1)))\
))
endef

define is-old-apple-clang
$(strip $(and \
    $(call is-host-os-darwin),\
    $(call is-clang,$(1)),\
    $(filter 2,$(call clang-major-version,$(1)))\
))
endef

define is-not-old-apple-clang
$(if $(call is-old-apple-clang,$(1)),,yes)
endef

# $1: source file
define objfile
$(OBJDIR)/$(1).o
endef

# $1: source file
define language
$(strip $(or \
    $(if $(filter $(c-sources-masks),            $(1)),c),\
    $(if $(filter $(c++-sources-masks),          $(1)),c++),\
    $(if $(filter $(objective-c-sources-masks),  $(1)),objective-c),\
    $(if $(filter $(objective-c++-sources-masks),$(1)),objective-c++),\
    $(error Can not detect language: '$(1)')\
))
endef

# $1: source file
define compiler-flags
$(strip $(or \
    $(if $(filter c,            $(call language,$(1))),$(CFLAGS)),\
    $(if $(filter c++,          $(call language,$(1))),$(CXXFLAGS)),\
    $(if $(filter objective-c,  $(call language,$(1))),$(OBJCFLAGS) $(CFLAGS)),\
    $(if $(filter objective-c++,$(call language,$(1))),$(OBJCFLAGS) $(CXXFLAGS)),\
    $(error Unknown language: '$(call language,$(1))')\
))
endef

#=================================================================================

CC ?= cc

TESTDIR := $(shell pwd)

ifeq (,$(strip $(SRCFILES)))
$(error SRCFILES are not defined)
endif

ifeq (,$(strip $(VPATH)))
VPATH := ../jni
endif

ifeq (,$(strip $(CXXFLAGS)))
CXXFLAGS := $(CFLAGS)
endif

ifeq (,$(filter -std=%,$(CFLAGS) $(CXXFLAGS)))
CXXFLAGS += -std=$(if $(call is-old-macosx),c++98,$(c++11))
endif

ifneq (,$(and $(call is-clang,$(CC)),$(call is-not-old-apple-clang,$(CC)),$(if $(filter -stdlib=%,$(CXXFLAGS)),,yes)))
CXXFLAGS += -stdlib=libc++
LDFLAGS  += -stdlib=libc++
endif

OBJCFLAGS := -f$(objc-runtime)-runtime
OBJCFLAGS += $(if $(call is-clang,$(CC)),-fblocks)

OBJDIR := obj/$(CC)
OBJFILES := $(foreach __f,$(filter $(all-sources-masks),$(SRCFILES)),$(call objfile,$(__f)))

TARGETDIR := bin/$(CC)
TARGETNAME := $(or $(strip $(TARGETNAME)),$(if $(filter 1,$(words $(SRCFILES))),$(addsuffix .bin,$(SRCFILES)),test))
TARGET := $(TARGETDIR)/$(TARGETNAME)

.PHONY: test
test: $(TARGET)
	$(if $(call is-true,$(BUILD_ONLY)),true,$(realpath $(TARGET)))

.PHONY: clean
clean:
	@rm -Rf $(TARGETDIR) $(OBJDIR)
	@rmdir $$(dirname $(TARGETDIR)) 2>/dev/null || true
	@rmdir $$(dirname $(OBJDIR)) 2>/dev/null || true

$(TARGET): $(OBJFILES) | $(dir $(TARGET))
	$(strip \
		$(CC) \
		$(if $(has-objective-c-sources),\
			-f$(objc-runtime)-runtime \
		)\
		$(LDFLAGS) \
		-o $@ $^ \
		$(LDLIBS) \
		$(if $(has-c++-sources),\
			-lstdc++ \
		)\
		$(if $(has-objective-c-sources),\
			$(if $(filter next,$(objc-runtime)),\
				-framework CoreFoundation \
				-framework Foundation \
			)\
			$(if $(and $(call is-clang,$(CC)),$(filter gnu,$(objc-runtime))),\
				-lBlocksRuntime \
			)\
			-lobjc \
		)\
		$(if $(call is-host-os-linux),\
			-lpthread -lrt \
		)\
		-lm -ldl \
	)

$(dir $(TARGET)):
	mkdir -p $@

define add-compile-rule
$$(call objfile,$(1)): $(1) | $$(dir $$(call objfile,$(1))) $$(PRETEST)
	$$(strip \
		$$(CC) -x $$(call language,$(1)) \
		$$(if $$(filter -O%,$$(call compiler-flags,$(1))),,-O0) \
		$$(call compiler-flags,$(1)) \
		-c -o $$@ $$^ \
	)

ifneq (yes,$$(mkdir_rule_created.$$(dir $$(call objfile,$(1)))))

$$(dir $$(call objfile,$(1))):
	mkdir -p $$@

$$(eval mkdir_rule_created.$$(dir $$(call objfile,$(1))) := yes)
endif

endef

$(foreach __f,$(SRCFILES),\
    $(eval $(call add-compile-rule,$(__f)))\
)
