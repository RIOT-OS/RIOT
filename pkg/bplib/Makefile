PKG_NAME    = bplib
PKG_URL     = https://github.com/nasa/bplib
# Original bplibs 7.0.2 release
PKG_VERSION = 5eda6abc1f2c4d7e1aea0a5681992e9e717b60bf
PKG_LICENSE = Apache License 2.0

include $(RIOTBASE)/pkg/pkg.mk



ifneq (,$(filter bplib_default_fwp,$(USEMODULE)))
SRC += $(RIOTPKG)/bplib/fwp/bplib_riot_fwp.c
endif

SUBMODS := $(filter-out $(IGNORE),$(filter bplib_%,$(USEMODULE)))


all: $(BINDIR)/libbplib.a $(SUBMODS)

bplib_default_fwp:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/fwp

bplib_nc:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/nc

bplib_init:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/init

bplib_stor_vfs:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/stor_vfs

bplib_cla_ble:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/cla/ble

bplib_cla_udp:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/cla/udp

$(BINDIR)/libbplib.a: $(PKG_BUILD_DIR)/libbplib.a
	@cp $< $@

$(PKG_BUILD_DIR)/libbplib.a:
	+"$(MAKE)" -C $(PKG_SOURCE_DIR) -f riot.mk BUILDDIR=$(PKG_BUILD_DIR)


#include $(RIOTBASE)/Makefile.base