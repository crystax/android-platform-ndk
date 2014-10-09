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
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.m,%.o,$(SRCFILES)))

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

$(TARGETDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

$(TARGET): $(OBJFILES) | $(TARGETDIR)
	$(CC) -o $@ $^ $(if $(filter yes,$(LINUX)),,-framework CoreFoundation) -lobjc

$(OBJDIR)/%.o: %.m | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
