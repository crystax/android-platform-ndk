include ../common.mk

CXXFLAGS += $(if $(and $(call is-host-os-darwin),$(call is-clang,$(CXX))),,-pthread)

include $(or $(NDK),../../../..)/tests/onhost.mk
