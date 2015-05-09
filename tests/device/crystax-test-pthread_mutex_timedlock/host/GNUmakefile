include ../common.mk

ifneq (,$(shell uname -s | grep -iq linux && echo yes))
CFLAGS := -D_GNU_SOURCE=1
LDLIBS := -lpthread
endif

include $(or $(NDK),../../../..)/tests/onhost.mk
