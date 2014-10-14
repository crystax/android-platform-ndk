include ../common.mk

is-old-apple-clang := $(strip $(and \
    $(shell $(CC) --version | grep -iq clang && echo yes),\
    $(shell uname -s | grep -iq darwin && echo yes),\
    $(shell test $$(sw_vers -productVersion | awk -F. '{print $$1 * 10000 + $$2 * 100 + $$3}') -lt 100900 && echo yes)\
))

ifneq (,$(is-old-apple-clang))

.PHONY: test
test:
	@echo "C++11 tests disabled for old Apple's Clang"

else

pthread-switch = $(if $(and $(call is-host-os-darwin),$(call is-clang,$(CXX))),,-pthread)

CXXFLAGS += $(pthread-switch)
LDFLAGS  += $(pthread-switch)

include $(or $(NDK),../../../..)/tests/onhost.mk
endif
