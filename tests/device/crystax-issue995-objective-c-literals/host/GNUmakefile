include ../common.mk
ifneq (,$(shell uname -s | grep -i darwin))
include $(or $(NDK),../../../..)/tests/onhost.mk
else
.PHONY: test
test:
	@echo "=== Skip on this OS"
endif
