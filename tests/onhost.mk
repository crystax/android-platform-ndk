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

define compiler-type
$(strip $(if $(strip $(1)),\
    $(if $(shell $(1) --version 2>/dev/null | grep -iq "\(gcc\|Free Software Foundation\)" && echo yes),\
        gcc,\
        $(if $(shell $(1) --version 2>/dev/null | grep -iq "\(clang\|llvm\)" && echo yes),\
            clang,\
            $(error Can not detect compiler type of '$(1)')\
        )\
    ),\
    $(error Usage: call c++-compiler-type,compiler)\
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
    $(shell $(1) -dumpversion 2>/dev/null | awk -F. '{print $$1}')\
))
endef

define gcc-minor-version
$(strip $(if $(call is-gcc,$(1)),\
    $(shell $(1) -dumpversion 2>/dev/null | awk -F. '{print $$2}')\
))
endef

define gcc-version
$(strip $(if $(strip $(1)),\
    $(if $(call is-gcc,$(1)),$(call gcc-major-version,$(1)).$(call gcc-minor-version,$(1))),\
    $(error Usage: call gcc-version,cc)\
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
$(if $(and $(call is-clang,$(CC)),$(call is-host-os-darwin)),next,gnu)
endef

define has-c-sources
$(if $(filter %.c %.m,$(SRCFILES)),yes)
endef

define has-objective-c-sources
$(if $(filter %.m %.mm,$(SRCFILES)),yes)
endef

define has-c++-sources
$(if $(filter %.cpp %.cc %.mm,$(SRCFILES)),yes)
endef

# $1: regexp
# $2: string
define match
$(shell echo $(2) | grep -iq $(1) && echo yes)
endef

#=================================================================================

CC ?= cc

TESTDIR := $(shell pwd)

is-device-test := $(shell test -d $(dir $(lastword $(MAKEFILE_LIST)))/device/$(notdir $(realpath $(TESTDIR)/..)) && echo yes)

ifeq (,$(wildcard $(TESTDIR)/DISABLED))
is-test-disabled :=
else
is-test-disabled := $(strip \
    $(eval __disabled := $(shell cat DISABLED 2>/dev/null))\
    $(or \
        $(call match,"\<$(call host-os)\>",$(__disabled)),\
        $(and $(call is-clang,$(CC)),$(call match,"\<clang\>",$(__disabled))),\
        $(and $(call is-gcc,$(CC)),$(call match,"\<gcc\>",$(__disabled))),\
        $(and $(call is-gcc,$(CC)),$(call match,"\<$(subst .,\.,gcc-$(call gcc-version,$(CC)))\>",$(__disabled)))\
    )\
)
endif

ifeq (,$(strip $(SRCFILES)))
$(error SRCFILES are not defined)
endif

ifeq (,$(strip $(VPATH)))
VPATH := ../jni
endif

ifeq (,$(strip $(CXXFLAGS)))
CXXFLAGS := $(CFLAGS)
endif

ifeq (,$(filter -std=%,$(CXXFLAGS)))
CXXFLAGS += -std=$(c++11)
endif

ifneq (,$(and $(call is-clang,$(CC)),$(if $(filter -stdlib=%,$(CXXFLAGS)),,yes)))
CXXFLAGS += -stdlib=libc++
LDFLAGS  += -stdlib=libc++
endif

OBJCFLAGS := -f$(objc-runtime)-runtime
OBJCFLAGS += $(if $(call is-clang,$(CC)),-fblocks)

OBJDIR := obj/$(CC)
OBJFILES := $(strip $(addprefix $(OBJDIR)/,\
    $(patsubst   %.c,%.o,$(filter   %.c,$(SRCFILES)))\
    $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRCFILES)))\
    $(patsubst   %.m,%.o,$(filter   %.m,$(SRCFILES)))\
    $(patsubst  %.mm,%.o,$(filter  %.mm,$(SRCFILES)))\
))

TARGETDIR := bin/$(CC)
TARGETNAME := $(or $(strip $(TARGETNAME)),test)
TARGET := $(TARGETDIR)/$(TARGETNAME)

.PHONY: test
ifneq (,$(is-test-disabled))
test:
	@echo "On-host test disabled"
else
test: $(TARGET)
ifeq (yes,$(is-device-test))
	$(realpath $(TARGET))
endif
endif

.PHONY: clean
clean:
	@rm -Rf $(TARGETDIR) $(OBJDIR)
	@rmdir $$(dirname $(TARGETDIR)) 2>/dev/null || true
	@rmdir $$(dirname $(OBJDIR)) 2>/dev/null || true

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJFILES) | $(TARGETDIR)
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
			)\
			$(if $(and $(call is-clang,$(CC)),$(filter gnu,$(objc-runtime))),\
				-lBlocksRuntime \
			)\
			-lobjc \
		)\
		-lm -ldl \
	)

ifneq (,$(PRETEST))
$(OBJFILES): | $(PRETEST)
endif

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) -x c $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) -x c++  $(CXXFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.m | $(OBJDIR)
	$(CC) -x objective-c $(OBJCFLAGS) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.mm | $(OBJDIR)
	$(CC) -x objective-c++ $(OBJCFLAGS) $(CXXFLAGS) -c -o $@ $^
