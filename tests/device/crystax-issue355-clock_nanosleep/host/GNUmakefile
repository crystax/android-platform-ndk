include ../common.mk

LINUX := $(shell uname -s | grep -iq linux && echo yes)

CC = gcc

VPATH := ../jni

OBJDIR := obj/$(CC)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CC)
TARGET := $(TARGETDIR)/test

.PHONY: test
ifneq (yes,$(LINUX))
test:
else
test: $(TARGET)
	$<
endif

.PHONY: build
ifneq (yes,$(LINUX))
build:
else
build: $(TARGET)
endif

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
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
