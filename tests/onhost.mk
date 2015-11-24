ifeq (,$(strip $(ONHOST_FUNCTIONS_INCLUDED_65010bdaed29436187034ed331bd56e9)))
include $(dir $(lastword $(MAKEFILE_LIST)))/onhost-functions.mk
endif

#=======================================================================================================================

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

#=======================================================================================================================

CC ?= cc

ifeq (,$(strip $(SRCFILES)))
$(error SRCFILES are not defined)
endif

ifeq (,$(strip $(VPATH)))
VPATH := ../jni
endif

ifeq (,$(strip $(CFLAGS)))
CFLAGS := $(space)
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
test: do-test

.PHONY: clean
clean:
	@rm -Rf $(TARGETDIR) $(OBJDIR)
	@rmdir $$(dirname $(TARGETDIR)) 2>/dev/null || true
	@rmdir $$(dirname $(OBJDIR)) 2>/dev/null || true

override SKIP := $(or \
    $(strip $(if \
        $(or \
            $(and \
                $(call is-gcc,$(CC)),\
                $(call is-version-less,$(call gcc-version,$(CC)),4.6)\
            ),\
            $(and \
                $(call is-clang,$(CC)),\
                $(call is-version-less,$(call clang-version,$(CC)),$(if $(call is-apple-clang,$(CC)),4.0,3.3))\
            )\
        ),\
        '$(CC)' is too old ($(call cc-type,$(CC))-$(call cc-version,$(CC))) \
    )),\
    $(strip $(if \
        $(and \
            $(call is-host-os-darwin),\
            $(call has-objective-c-sources),\
            $(or \
                $(call is-not-apple-clang,$(CC)),\
                $(call is-version-less,$(call clang-version,$(CC)),5.0)\
            )\
        ),\
        '$(CC)' do not support modern Objective-C \
    ))\
)

ifneq (,$(strip $(SKIP)))

.PHONY: do-test
do-test:
	@echo "SKIP on-host testing: $(SKIP)"

else

.PHONY: test
do-test: $(TARGET)
	$(if $(call is-true,$(BUILD_ONLY)),true,$(strip $(LAUNCHER) $(realpath $(TARGET))))

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

endif
