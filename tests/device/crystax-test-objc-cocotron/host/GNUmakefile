MYDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

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

include ../common.mk

CFLAGS += -I../jni

define add-test-rule
.PHONY: test
test: test-$(1)

.PHONY: test-$(1)
test-$(1):
	@+$(MAKE) -f $(MYDIR)/test.mk SRCFILES=test-$(1).m CFLAGS="$(CFLAGS)" TARGETNAME=test-$(1)

.PHONY: clean-$(1)
clean-$(1):
	@+$(MAKE) -f $(MYDIR)/test.mk clean SRCFILES=test-$(1).m

.PHONY: clean
clean: clean-$(1)
endef

$(foreach __t,$(CTESTS),\
    $(eval $(call add-test-rule,$(__t)))\
)

endif
