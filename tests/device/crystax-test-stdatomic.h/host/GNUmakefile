include ../common.mk
include $(or $(NDK),../../../..)/tests/onhost-functions.mk

ifneq (,$(and $(call is-gcc,$(CC)),$(call is-version-less,$(call gcc-version,$(CC)),4.9)))
.PHONY: test
test:
	@echo "SKIP on-host testing for gcc-$(call gcc-version,$(CC))"
else
include $(or $(NDK),../../../..)/tests/onhost.mk
endif
