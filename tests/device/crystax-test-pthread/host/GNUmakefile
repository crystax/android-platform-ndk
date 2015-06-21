MYDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

include $(MYDIR)/../common.mk

define add-test-rule
.PHONY: test
test: test-$(1)

.PHONY: test-$(1)
test-$(1):
	@+$(MAKE) -f $(MYDIR)/test.mk SRCFILES=$(1).c CFLAGS="$(CFLAGS)"

.PHONY: clean-$(1)
clean-$(1):
	@+$(MAKE) -f $(MYDIR)/test.mk clean SRCFILES=$(1).c

.PHONY: clean
clean: clean-$(1)
endef

$(foreach __t,$(CTESTS),\
    $(eval $(call add-test-rule,$(__t)))\
)
