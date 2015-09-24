include ../common.mk

is-macosx := $(shell uname -s | grep -iq darwin && echo yes)

is-not-macosx := $(if $(is-macosx),,yes)

is-old-macosx := $(strip $(and \
    $(is-macosx),\
    $(shell test $$(sw_vers -productVersion | awk -F. '{print $$1 * 10000 + $$2 * 100 + $$3}') -lt 100900 && echo yes)\
))

is-gcc := $(shell echo __clang__ | $(CC) -x c -E - | tail -n 1 | grep -q __clang__ && echo yes)

ifneq (,$(or $(is-not-macosx),$(is-old-macosx),$(is-gcc)))

.PHONY: test
test:
	@echo "These tests are disabled on this system"

else

include $(or $(NDK),../../../..)/tests/onhost.mk

endif
