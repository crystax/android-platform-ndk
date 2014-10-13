include ../common.mk

LINUX := $(shell uname -s | grep -iq linux && echo yes)

ifeq (yes,$(shell $(CC) --version 2>/dev/null | grep -iq gcc && echo yes))
CFLAGS += -Wno-unused-parameter
else
ifeq (yes,$(LINUX))
CFLAGS += -Wno-unused-parameter
endif
endif

include $(or $(NDK),../../../..)/tests/onhost.mk
