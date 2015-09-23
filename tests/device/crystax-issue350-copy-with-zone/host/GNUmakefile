include ../common.mk

is-gcc-on-os-x-pre-10-9 := $(strip $(and \
    $(shell $(CC) --version | grep -iq gcc && echo yes),\
    $(shell uname -s | grep -iq darwin && echo yes),\
    $(shell test $$(sw_vers -productVersion | awk -F. '{print $$1 * 10000 + $$2 * 100 + $$3}') -lt 100900 && echo yes)\
))

is-linux := $(shell uname -s | grep -iq linux && echo yes)

ifneq (,$(is-gcc-on-os-x-pre-10-9))

.PHONY: test
test:
	@echo "Skipped for GCC on OS X < 10.9"

else
ifneq (,$(is-linux))

.PHONY: test
test:
	@echo "Skipped for Linux"

else

CFLAGS += $(strip \
    $(if \
        $(shell $(CC) --version | grep -iq gcc && echo yes),\
        -Wno-unused-parameter\
    )\
)

include $(or $(NDK),../../../..)/tests/onhost.mk

endif
endif
