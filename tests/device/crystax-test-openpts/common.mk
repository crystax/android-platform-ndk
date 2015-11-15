ifeq (,$(strip $(NDK)))
$(error NDK is not defined!)
endif

OPENPTS := $(realpath $(NDK)/sources/crystax/tests/openpts)

ifeq (,$(strip $(OPENPTS)))
$(error Can not locate OpenPTS sources!)
endif

TSUITES := $(wildcard $(OPENPTS)/conformance/interfaces/*)

FILES := $(sort $(foreach __t,$(TSUITES),\
    $(foreach __f,\
        $(patsubst $(__t)/%,%,$(wildcard $(__t)/*.c)),\
        $(if $(filter 2,$(words $(subst -, ,$(__f)))),$(__f))\
    )\
))

MAJORS := $(shell echo $(strip $(sort $(foreach __f,$(FILES),\
    $(firstword $(subst -, ,$(__f))))) | \
    tr ' ' '\n' | grep '^[0-9]' | sort -n | uniq \
))
MINORS := $(shell echo $(strip $(sort $(foreach __f,$(FILES),\
    $(lastword $(subst -, ,$(patsubst %.c,%,$(__f)))))) | \
    tr ' ' '\n' | grep '^[0-9]' | sort -n | uniq \
))

CTESTS := $(strip $(sort \
    $(foreach __f,\
        $(foreach __t,$(TSUITES),\
            $(foreach __major,$(MAJORS),\
                $(foreach __minor,$(MINORS),\
                    $(wildcard $(__t)/$(__major)-$(__minor).c)\
                )\
            )\
        ),\
        $(patsubst %.c,%,$(__f))\
    )\
))

CFLAGS := -std=gnu99
CFLAGS += -Wall -Werror
CFLAGS += -I$(OPENPTS)/include
