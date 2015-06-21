ifeq (,$(strip $(NDK)))
$(error NDK is not defined!)
endif

OPENPTS := $(or \
    $(wildcard $(NDK)/sources/crystax/tests/openpts),\
    $(error Can not locate OpenPTS sources)\
)

OPENPTS := $(realpath $(OPENPTS))

TSUITES := pthread_attr_init

CTESTS := $(strip \
    $(foreach __f,\
        $(sort $(filter-out %/testfrmw.c,$(foreach __t,$(TSUITES),$(wildcard $(OPENPTS)/conformance/interfaces/$(__t)/*-*.c)))),\
        $(patsubst %.c,%,$(__f))\
    )\
)

CFLAGS := -Wall -Werror
CFLAGS += -UNDEBUG
CFLAGS := -I$(OPENPTS)/include
