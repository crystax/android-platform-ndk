include ../common.mk

CFLAGS := -D_GNU_SOURCE=1

ifneq (,$(shell uname -s | grep -iq linux && echo yes))
LDLIBS := -lpthread
endif

include $(or $(NDK),../../../..)/tests/onhost.mk
