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

define std-c++11-switch
$(strip \
    $(if \
        $(and \
            $(call is-gcc,$(CXX)),\
            $(filter 4,$(call gcc-major-version,$(CXX))),\
            $(filter 6,$(call gcc-minor-version,$(CXX)))\
        ),\
        -std=c++0x,\
        -std=c++11\
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

define join-with
$(if $(strip $(2)),$(call head,$(2))$(if $(strip $(call tail,$(2))),$(1)$(call join-with,$(1),$(call tail,$(2)))))
endef

ifneq (a-b-c,$(call join-with,-,a b c))
$(error Function 'join-with' broken)
endif

#=================================================================================

CC ?= cc
CXX ?= c++

TESTDIR := $(shell pwd)

ifeq (,$(strip $(C_ENABLED)))
C_ENABLED := $(if $(filter %.c %.m,$(SRCFILES)),yes)
endif

ifeq (,$(strip $(CXX_ENABLED)))
CXX_ENABLED := $(if $(filter %.cpp %.cc %.mm,$(SRCFILES)),yes)
endif

is-test-disabled :=
ifneq (,$(wildcard $(TESTDIR)/DISABLED))

is-test-disabled := $(or $(is-test-disabled),$(shell grep -iq "\<$(call host-os)\>" DISABLED && echo yes))

ifneq (,$(call is-true,$(C_ENABLED)))
ifneq (,$(call is-gcc,$(CC)))
is-test-disabled := $(or $(is-test-disabled),$(shell grep -iq "\<gcc-$(subst .,\.,$(call gcc-version,$(CC)))\>" DISABLED && echo yes))
endif
endif

ifneq (,$(call is-true,$(CXX_ENABLED)))
ifneq (,$(call is-gcc,$(CXX)))
is-test-disabled := $(or $(is-test-disabled),$(shell grep -iq "\<gcc-$(subst .,\.,$(call gcc-version,$(CXX)))\>" DISABLED && echo yes))
endif
endif

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
CXXFLAGS += $(call std-c++11-switch)
endif

ifneq (,$(filter %.m %.mm,$(SRCFILES)))
ifneq (,$(filter darwin,$(call host-os)))
LDLIBS += -framework CoreFoundation
endif
LDLIBS += -lobjc
endif

OBJDIR := obj/$(call join-with,-,$(if $(call is-true,$(C_ENABLED)),$(CC)) $(if $(call is-true,$(CXX_ENABLED)),$(CXX)))
OBJFILES := $(strip $(addprefix $(OBJDIR)/,\
    $(patsubst   %.c,%.o,$(filter   %.c,$(SRCFILES)))\
    $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRCFILES)))\
    $(patsubst   %.m,%.o,$(filter   %.m,$(SRCFILES)))\
    $(patsubst  %.mm,%.o,$(filter  %.mm,$(SRCFILES)))\
))

TARGETDIR := bin/$(call join-with,-,$(if $(call is-true,$(C_ENABLED)),$(CC)) $(if $(call is-true,$(CXX_ENABLED)),$(CXX)))
TARGET := $(TARGETDIR)/test

.PHONY: test
ifneq (,$(is-test-disabled))
test:
	@echo "On-host test disabled"
else
test: $(TARGET)
	cd $$(dirname $(TARGET)) && ./$$(basename $(TARGET))
endif

.PHONY: clean
clean:
	@rm -Rf $(TARGETDIR) $(OBJDIR)

ifeq (yes,$(call is-true,$(C_ENABLED)))
.PHONY: c-enabled
c-enabled:
	@true
endif

ifeq (yes,$(call is-true,$(CXX_ENABLED)))
.PHONY: c++-enabled
c++-enabled:
	@true
endif

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJFILES) | $(TARGETDIR)
	$(if $(call is-true,$(CXX_ENABLED)),$(CXX),$(CC)) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.m | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.mm | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^
