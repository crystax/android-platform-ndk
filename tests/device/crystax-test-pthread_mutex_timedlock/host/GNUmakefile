include ../common.mk

CC = gcc

DARWIN := $(shell uname -s | grep -iq darwin && echo yes)

VPATH := ../jni

OBJDIR := obj/$(CC)
OBJFILES := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRCFILES)))

TARGETDIR := bin/$(CC)
TARGET := $(TARGETDIR)/test-pthread_mutex_timedlock

CFLAGS += -D_GNU_SOURCE=1

.PHONY: test
ifeq (yes,$(DARWIN))
test:
else
test: $(TARGET)
	$<
endif

.PHONY: build
ifeq (yes,$(DARWIN))
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
	$(CC) -o $@ $^ -lpthread -lrt

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $^
