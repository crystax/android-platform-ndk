include ../common.mk

CC = gcc

VPATH := ../jni

OBJDIR := obj/$(CC)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CC)
TARGET := $(TARGETDIR)/test-c99-stdlib

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
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
