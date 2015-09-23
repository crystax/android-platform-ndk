include ../common.mk

ifneq (,$(shell uname -s | grep -i linux))
.PHONY: test
test:
	@echo "Skipped for Linux"

else
include $(or $(NDK),../../../..)/tests/onhost.mk
endif
