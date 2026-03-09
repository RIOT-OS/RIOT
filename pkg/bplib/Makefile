PKG_NAME    = bplib
PKG_URL     = https://github.com/nasa/bplib
# Original bplibs 7.0.2 release
PKG_VERSION = 5eda6abc1f2c4d7e1aea0a5681992e9e717b60bf
PKG_LICENSE = Apache License 2.0

include $(RIOTBASE)/pkg/pkg.mk

# These are the bplib pseudomodules
IGNORE  := bplib_walltime_available bplib_include_nc_telemetry bplib_include_as
IGNORE  += bplib_no_vfs

SUBMODS := $(filter-out $(IGNORE),$(filter bplib_%,$(USEMODULE)))

all: $(BINDIR)/libbplib.a $(SUBMODS)

# ------------------------- General helper submodules --------------------------
bplib_fwp:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/fwp

bplib_nc:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/nc

bplib_init:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/init

# ----------------------------- Storage modules --------------------------------
bplib_stor_vfs_ordered:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/storage/vfs/ordered

bplib_stor_vfs_unordered:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/storage/vfs/unordered

bplib_stor_void:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/storage/mem/void

# -------------------------------- CLA modules ---------------------------------
bplib_cla_ble:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/cla/ble

bplib_cla_udp:
	+"$(MAKE)" -C $(RIOTPKG)/bplib/cla/udp

# ------------------------------- bplib library --------------------------------
$(BINDIR)/libbplib.a: $(PKG_BUILD_DIR)/libbplib.a
	@cp $< $@

$(PKG_BUILD_DIR)/libbplib.a:
	+"$(MAKE)" -C $(PKG_SOURCE_DIR) -f riot.mk BUILDDIR=$(PKG_BUILD_DIR)
