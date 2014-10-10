include ../common.mk
include $(or $(NDK),../../../..)/tests/onhost.mk

DISABLED := $(if $(or $(call is-gcc-version,4,6),$(call is-gcc-version,4,7)),yes)

CXXFLAGS := $(call std-c++11-switch) $(CFLAGS)
CXXFLAGS += $(if $(and $(call is-host-os-darwin),$(call is-cxx-clang)),,-pthread)

VPATH := ../jni

OBJDIR := obj/$(CXX)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CXX)
TARGET := $(TARGETDIR)/test-c++11

.PHONY: test
ifeq (yes,$(DISABLED))
test:
else
test: $(TARGET)
	$<
endif

.PHONY: build
ifeq (yes,$(DISABLED))
build:
else
build: $(TARGET)
endif

.PHONY: clean
clean:
	rm -Rf $(TARGETDIR) $(OBJDIR)

.PHONY: c++-enabled
c++-enabled:
	@true

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJFILES) | $(TARGETDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^
