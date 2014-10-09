include ../common.mk

CXX = g++

define is-cxx-clang
$(shell $(CXX) --version | grep -iq clang && echo yes)
endef

CXXFLAGS := -std=gnu++11 $(if $(call is-cxx-clang),,-pthread) $(CFLAGS)

VPATH := ../jni

OBJDIR := obj/$(CXX)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CXX)
TARGET := $(TARGETDIR)/test-c++11

.PHONY: test
test: $(TARGET)
	$<

.PHONY: build
build: $(TARGET)

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
