include ../common.mk

CC ?= clang

ifeq (yes,$(shell $(CC) --version 2>/dev/null | grep -iq gcc && echo yes))
CFLAGS += -Wno-unused-parameter
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
	$(CC) -o $@ $^ -framework CoreFoundation -lobjc

$(OBJDIR)/%.o: %.m | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
