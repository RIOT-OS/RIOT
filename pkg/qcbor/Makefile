PKG_NAME    = qcbor
PKG_URL     = https://github.com/laurencelundblade/QCBOR
# v1.6
PKG_VERSION = 590a23daf65af068cee81555c597063150e23539
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-type-limits

# The newlib implementation of some ESP variants does not implement the
# `feclearexcept` and `fetestexcept` functions from `fenv.h` used by
# QCBOR. Disabling the HW FPU support works around this issue.
ifneq (,$(filter esp8266 esp32s2, $(CPU_FAM)))
  CFLAGS += -DQCBOR_DISABLE_FLOAT_HW_USE
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
