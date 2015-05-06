include ../common.mk

LINUX := $(shell uname -s | grep -iq linux && echo yes)

is-old-macosx := $(strip $(and \
    $(if $(strip $(LINUX)),,yes),\
    $(shell uname -s | grep -iq darwin && echo yes),\
    $(shell test $$(sw_vers -productVersion | awk -F. '{print $$1 * 10000 + $$2 * 100 + $$3}') -lt 100900 && echo yes)\
))

ifneq (,$(is-old-macosx))

.PHONY: test
test:
	@echo "These tests are disabled for old Apple systems"

else

ifeq (yes,$(shell $(CC) --version 2>/dev/null | grep -iq gcc && echo yes))
CFLAGS += -Wno-unused-parameter
else
ifeq (yes,$(LINUX))
CFLAGS += -Wno-unused-parameter
endif
endif

include $(or $(NDK),../../../..)/tests/onhost.mk

endif
