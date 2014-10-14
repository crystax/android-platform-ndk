include ../common.mk

pthread-switch = $(if $(and $(call is-host-os-darwin),$(call is-clang,$(CXX))),,-pthread)

CXXFLAGS += $(pthread-switch)
LDFLAGS  += $(pthread-switch)

include $(or $(NDK),../../../..)/tests/onhost.mk
