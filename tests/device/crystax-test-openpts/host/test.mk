PRETEST := build-launcher

include $(or $(NDK),../../../..)/tests/onhost.mk

LAUNCHERBIN := $(TARGETDIR)/t0
LAUNCHER := $(LAUNCHERBIN) 240

.PHONY: build-launcher
build-launcher: $(LAUNCHERBIN)

$(dir $(LAUNCHERBIN)):
	mkdir -p $@

$(LAUNCHERBIN): $(OPENPTS)/t0.c | $(dir $(LAUNCHERBIN))
	$(CC) -O2 -o $@ $<
