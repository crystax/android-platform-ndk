include ../common.mk
include $(or $(NDK),../../../..)/tests/onhost.mk

CXXFLAGS := $(call std-c++11-switch) $(CFLAGS)

VPATH := ../jni

OBJDIR := obj/$(CC)
OBJFILES := $(strip $(addprefix $(OBJDIR)/,\
    $(patsubst   %.c,%.o,$(filter   %.c,$(SRCFILES)))\
    $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRCFILES)))\
))

TARGETDIR := bin/$(CC)
TARGET := $(TARGETDIR)/test-sincos

.PHONY: test
test: $(TARGET)
	$<

.PHONY: build
build: $(TARGET)

.PHONY: clean
clean:
	rm -Rf $(TARGETDIR) $(OBJDIR)

.PHONY: c-enabled
c-enabled:
	@true

.PHONY: c++-enabled
c++-enabled:
	@true

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJFILES) | $(TARGETDIR)
	$(CXX) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^
