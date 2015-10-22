ifneq (yes,$(shell which grep >/dev/null 2>&1 && echo yes))
$(error No 'grep' tool found)
endif

ifneq (yes,$(shell which awk >/dev/null 2>&1 && echo yes))
$(error No 'awk' tool found)
endif

empty :=
space := $(empty) $(empty)
comma := ,

define check-function
$(if $(strip $(2)),,$(error INTERNAL ERROR: function '$(1)' is broken!))
endef

define not
$(if $(strip $(1)),,yes)
endef

define is-empty
$(if $(strip $(1)),,yes)
endef

define head
$(firstword $(1))
endef

$(call check-function,head,$(filter a,$(call head,a b c)))

define tail
$(wordlist 2,$(words $(1)),$(1))
endef

$(call check-function,tail,$(filter 2,$(words $(call tail,a b c))))
$(call check-function,tail,$(filter b,$(word 1,$(call tail,a b c))))
$(call check-function,tail,$(filter c,$(word 2,$(call tail,a b c))))

define str.eq
$(or $(call is-empty,$(1) $(2)),$(if $(call is-empty,$(2)),,$(if $(strip $(subst $(1),,$(2))),,yes)))
endef

$(call check-function,str.eq,$(call str.eq,asd,asd))
$(call check-function,str.eq,$(call str.eq,amaimiblaend12,amaimiblaend12))
$(call check-function,str.eq,$(call str.eq,,))
$(call check-function,str.eq,$(call not,$(call str.eq,asd,dsa)))
$(call check-function,str.eq,$(call not,$(call str.eq,km92uenijna,)))
$(call check-function,str.eq,$(call not,$(call str.eq,,lakjoiln)))

define str.le
$(or $(call is-empty,$(1)),$(if $(call is-empty,$(2)),,$(call str.eq,$(word 1,$(sort $(1) $(2))),$(1))))
endef

$(call check-function,str.le,$(call str.le,,,))
$(call check-function,str.le,$(call str.le,,abc))
$(call check-function,str.le,$(call str.le,abc,abc))
$(call check-function,str.le,$(call str.le,abc,bbc))
$(call check-function,str.le,$(call str.le,1,1))
$(call check-function,str.le,$(call str.le,1,2))
$(call check-function,str.le,$(call not,$(call str.le,abc,)))
$(call check-function,str.le,$(call not,$(call str.le,bbc,abc)))
$(call check-function,str.le,$(call not,$(call str.le,9,5)))

define num.mklist-impl
$(strip \
    $(subst 0,0 ,\
    $(subst 1,1 ,\
    $(subst 2,2 ,\
    $(subst 3,3 ,\
    $(subst 4,4 ,\
    $(subst 5,5 ,\
    $(subst 6,6 ,\
    $(subst 7,7 ,\
    $(subst 8,8 ,\
    $(subst 9,9 ,\
    $(1)\
    ))))))))))\
)
endef

define check-isnum
$(strip \
    $(if $(filter 1,$(words $(1))),,$(error Usage: call num.isnum,number))\
    $(if $(filter-out 0 1 2 3 4 5 6 7 8 9,$(call num.mklist-impl,$(1))),$(error passed values is not a number: '$(1)'))\
)
endef

define num.mklist
$(strip \
    $(if $(filter 1,$(words $(1))),,$(error Usage: call num.mklist,number))\
    $(call check-isnum,$(1))\
    $(call num.mklist-impl,$(1))\
)
endef

$(call check-function,num.mklist,$(filter 3,$(words $(call num.mklist,975))))
$(call check-function,num.mklist,$(filter 9,$(word 1,$(call num.mklist,975))))
$(call check-function,num.mklist,$(filter 7,$(word 2,$(call num.mklist,975))))
$(call check-function,num.mklist,$(filter 5,$(word 3,$(call num.mklist,975))))
$(call check-function,num.mklist,$(filter 3,$(words $(call num.mklist,975))))

define num.le
$(strip \
    $(if $(and $(filter 1,$(words $(1))),$(filter 1,$(words $(2)))),,$(error Usage: call num.le,number1,number2))\
    $(call check-isnum,$(1))\
    $(call check-isnum,$(2))\
    $(if $(call str.eq,$(words $(call num.mklist,$(1))),$(words $(call num.mklist,$(2)))),\
        $(call str.le,$(strip $(1)),$(strip $(2))),\
        $(call str.le,$(words $(call num.mklist,$(1))),$(words $(call num.mklist,$(2))))\
    )\
)
endef

$(call check-function,num.le,$(call num.le,0,0))
$(call check-function,num.le,$(call num.le,0,1))
$(call check-function,num.le,$(call num.le,1,1))
$(call check-function,num.le,$(call num.le,1,2))
$(call check-function,num.le,$(call num.le,1,11))
$(call check-function,num.le,$(call num.le,2,10))
$(call check-function,num.le,$(call num.le,57818273,57818274))
$(call check-function,num.le,$(call num.le,57818273,178182740))
$(call check-function,num.le,$(call not,$(call num.le,1,0)))
$(call check-function,num.le,$(call not,$(call num.le,10,5)))
$(call check-function,num.le,$(call not,$(call num.le,178182740,57818273)))

define num.gt
$(strip \
    $(if $(and $(filter 1,$(words $(1))),$(filter 1,$(words $(2)))),,$(error Usage: call num.gt,number1,number2))\
    $(call not,$(call num.le,$(1),$(2)))\
)
endef

define num.eq
$(strip \
    $(if $(and $(filter 1,$(words $(1))),$(filter 1,$(words $(2)))),,$(error Usage: call num.eq,number1,number2))\
    $(call check-isnum,$(1))\
    $(call check-isnum,$(2))\
    $(call str.eq,$(strip $(1)),$(strip $(2)))\
)
endef

define num.ge
$(or $(call num.gt,$(1),$(2)),$(call num.eq,$(1),$(2)))
endef

define num.lt
$(and $(call num.le,$(1),$(2)),$(call not,$(call num.eq,$(1),$(2))))
endef

# $1: major.minor
# $2: major.minor
# return: true if $1 < $2
define is-version-less
$(strip \
    $(if $(and $(filter 2,$(words $(subst ., ,$(1)))),$(filter 2,$(words $(subst ., ,$(2))))),,\
        $(error Usage: call is-version-less,major1.minor1,major2.minor2)\
    )\
    $(or \
        $(call num.lt,$(word 1,$(subst ., ,$(1))),$(word 1,$(subst ., ,$(2)))),\
        $(and \
            $(call num.eq,$(word 1,$(subst ., ,$(1))),$(word 1,$(subst ., ,$(2)))),\
            $(call num.lt,$(word 2,$(subst ., ,$(1))),$(word 2,$(subst ., ,$(2))))\
        )\
    )\
)
endef

$(call check-function,is-version-less,$(call is-version-less,1.0,1.1))
$(call check-function,is-version-less,$(call is-version-less,1.9,2.1))
$(call check-function,is-version-less,$(call is-version-less,2.9,2.10))

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
    $(filter 4.2,$(call gcc-major-version,$(1)).$(call gcc-minor-version,$(1)))\
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

define is-apple-clang
$(if $(filter __apple_build_version__,$(call preprocess,$(1),__apple_build_version__)),,yes)
endef

define is-not-apple-clang
$(if $(call is-apple-clang,$(1)),,yes)
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

empty :=
space := $(empty) $(empty)

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
        $(and \
            $(call is-host-os-linux),\
            $(call is-gcc,$(CC)),\
            $(call is-version-less,$(call gcc-major-version,$(CC)).$(call gcc-minor-version,$(CC)),4.6)\
        ),\
        '$(CC)' is too old ($(call gcc-major-version,$(CC)).$(call gcc-minor-version,$(CC))) \
    )),\
    $(strip $(if \
        $(and \
            $(call is-host-os-darwin),\
            $(call has-objective-c-sources)\
            $(or \
                $(call is-not-apple-clang,$(CC)),\
                $(call is-version-less,$(call clang-major-version,$(CC)).$(call clang-minor-version,$(CC)),5.0)\
            )\
        ),\
        '$(CC)' do not support modern Objective-C \
    ))\
)

ifneq (,$(SKIP))

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
