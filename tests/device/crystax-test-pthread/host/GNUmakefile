MYDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

include $(MYDIR)/../common.mk

CFLAGS += -O2 -D_POSIX_C_SOURCE=200112L

PREVIOUS_TARGETS :=

define add-test-rule
.PHONY: test
test: test-$(1)

.PHONY: test-$(1)
test-$(1): $(PREVIOUS_TARGETS)
	@+$(MAKE) -f $(MYDIR)/test.mk SRCFILES=$(1).c CFLAGS="$(CFLAGS)" OPENPTS="$(OPENPTS)"

PREVIOUS_TARGETS += test-$(1)

.PHONY: clean-$(1)
clean-$(1):
	@+$(MAKE) -f $(MYDIR)/test.mk clean SRCFILES=$(1).c OPENPTS="$(OPENPTS)"

.PHONY: clean
clean: clean-$(1)
endef

$(foreach __t,$(CTESTS),\
    $(eval $(call add-test-rule,$(__t)))\
)
