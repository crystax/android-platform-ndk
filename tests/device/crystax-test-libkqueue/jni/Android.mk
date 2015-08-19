LOCAL_PATH := $(call my-dir)

ifeq (,$(strip $(NDK)))
NDK := $(abspath $(LOCAL_PATH)/../../../..)
endif

TDIR := $(NDK)/sources/crystax/tests/libkqueue

include $(CLEAR_VARS)
LOCAL_MODULE     := crystax-test-libkqueue
LOCAL_SRC_FILES  := $(addprefix $(TDIR)/,\
	main.c   \
	kevent.c \
	test.c   \
	read.c   \
	signal.c \
	timer.c  \
	vnode.c  \
	user.c   \
)
LOCAL_CFLAGS     := -Wall -Wextra -Werror
include $(BUILD_EXECUTABLE)
