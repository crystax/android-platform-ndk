ifeq (,$(strip $(NDK)))
$(error NDK is not defined!)
endif

OPENPTS := $(realpath $(NDK)/sources/crystax/tests/openpts)

ifeq (,$(strip $(OPENPTS)))
$(error Can not locate OpenPTS sources!)
endif

TSUITES := $(addprefix conformance/interfaces/, \
	pthread_attr_destroy                        \
	pthread_attr_getdetachstate                 \
	pthread_attr_getinheritsched                \
	pthread_attr_getschedparam                  \
	pthread_attr_getschedpolicy                 \
	pthread_attr_getscope                       \
	pthread_attr_getstack                       \
	pthread_attr_getstackaddr                   \
	pthread_attr_getstacksize                   \
	pthread_attr_init                           \
	pthread_attr_setdetachstate                 \
	pthread_attr_setstack                       \
	pthread_attr_setstackaddr                   \
	pthread_attr_setstacksize                   \
)

FILES := $(sort $(foreach __t,$(TSUITES),\
    $(foreach __f,\
        $(patsubst $(OPENPTS)/$(__t)/%,%,$(wildcard $(OPENPTS)/$(__t)/*.c)),\
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

CTESTS := $(strip \
    $(foreach __f,\
        $(foreach __t,$(TSUITES),\
            $(foreach __major,$(MAJORS),\
                $(foreach __minor,$(MINORS),\
                    $(wildcard $(OPENPTS)/$(__t)/$(__major)-$(__minor).c)\
                )\
            )\
        ),\
        $(patsubst %.c,%,$(__f))\
    )\
)

CFLAGS := -std=gnu99
CFLAGS += -Wall -Werror
CFLAGS += -I$(OPENPTS)/include
