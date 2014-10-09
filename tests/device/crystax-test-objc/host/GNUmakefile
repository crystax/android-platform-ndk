include ../common.mk

CC ?= clang

LINUX := $(shell uname -s | grep -iq linux && echo yes)

ifeq (yes,$(shell $(CC) --version 2>/dev/null | grep -iq gcc && echo yes))
CFLAGS += -Wno-unused-parameter
else
ifeq (yes,$(LINUX))
CFLAGS += -Wno-unused-parameter
endif
endif

VPATH := ../jni

OBJDIR := obj/$(CC)
OBJFILES := $(strip $(addprefix $(OBJDIR)/,\
    $(patsubst  %.c,%.o,$(filter  %.c,$(SRCFILES)))\
    $(patsubst  %.m,%.o,$(filter  %.m,$(SRCFILES)))\
    $(patsubst %.mm,%.o,$(filter %.mm,$(SRCFILES)))\
))

TARGETDIR := bin/$(CC)
TARGET := $(TARGETDIR)/test

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
	$(CXX) -o $@ $^ $(if $(filter yes,$(LINUX)),,-framework CoreFoundation) -lobjc

$(OBJDIR)/%.o: %.m | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.mm | $(OBJDIR)
	$(CXX) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
