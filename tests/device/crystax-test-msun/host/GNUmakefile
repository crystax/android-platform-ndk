MYDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

include ../common.mk

CFLAGS += -I../jni

ifeq (yes,$(shell uname -s | grep -iq linux && echo yes))
CFLAGS += -D_GNU_SOURCE=1
endif

define add-test-rule
.PHONY: test
test: test-$(1)

.PHONY: test-$(1)
test-$(1):
	@$(MAKE) -f $(MYDIR)/test.mk SRCFILES=test-$(1).c CFLAGS="$(CFLAGS)" TARGETNAME=test-$(1)

.PHONY: clean-$(1)
clean-$(1):
	@$(MAKE) -f $(MYDIR)/test.mk clean SRCFILES=test-$(1).c

.PHONY: clean
clean: clean-$(1)
endef

$(foreach __t,$(TESTS),\
    $(eval $(call add-test-rule,$(__t)))\
)
