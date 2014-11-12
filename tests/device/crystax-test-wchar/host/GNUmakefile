MYDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

include ../common.mk

CFLAGS += -I../jni

define add-test-rule
.PHONY: test
test: test-$(1)

.PHONY: test-$(1)
test-$(1):
	@$(MAKE) -f $(MYDIR)/test.mk SRCFILES=test-$(1).$(2) CFLAGS="$(CFLAGS)" TARGETNAME=test-$(2)-$(1)

.PHONY: clean-$(1)
clean-$(1):
	@$(MAKE) -f $(MYDIR)/test.mk clean SRCFILES=test-$(1).$(2)

.PHONY: clean
clean: clean-$(1)
endef

$(foreach __t,$(TESTS),\
    $(eval $(call add-test-rule,$(__t),c))\
)
$(foreach __t,$(CXXTESTS),\
    $(eval $(call add-test-rule,$(__t),cpp))\
)
