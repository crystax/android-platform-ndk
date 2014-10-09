include ../common.mk
ifeq (,$(NDK))
NDK := $(shell cd ../../../.. && pwd)
endif
include $(NDK)/tests/onhost.mk

CXXFLAGS := $(call std-c++11-switch) $(CFLAGS)

VPATH := ../jni

OBJDIR := obj/$(CXX)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CXX)
TARGET := $(TARGETDIR)/test

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
