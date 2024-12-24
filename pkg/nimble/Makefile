PKG_NAME    = nimble
PKG_URL     = https://github.com/apache/mynewt-nimble.git
# tag: nimble_1_8_0_tag
PKG_VERSION = ca67e3015eec30eb97c853112441082d709e43ba
PKG_LICENSE = Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

TDIR = $(RIOTPKG)/$(PKG_NAME)
PDIR = $(PKG_SOURCE_DIR)

CFLAGS += -Wno-unused-but-set-variable

# NimBLE is not optimized for building with all (extra) warnings enabled. So for
# now, we disable a number of selected compiler warnings when building NimBLE.
CFLAGS += -Wno-extra -Wno-cast-align
ifeq (llvm,$(TOOLCHAIN))
# the static function `ble_ll_adv_active_chanset_is_sec()` in
# `nimble/controller/src/ble_ll_adv.c` isn't used in our compilation path, so
# tell LLVM/clang not to be so pedantic with this.
  CFLAGS += -Wno-unused-function
# Workaround, until https://github.com/apache/mynewt-nimble/pull/566 is merged
# upstream and the NimBLE version in RIOT is updated.
  CFLAGS += -Wno-sometimes-uninitialized
  CFLAGS += -Wno-parentheses-equality
# Backward compatibility with older versions of LLVM (when it doesn't know
# about the -Wno-foo, it won't throw any -Wfoo related warnings anyway)
  CFLAGS += -Wno-unknown-warning-option
endif

IGNORE := nimble_autoconn_% nimble_phy_% nimble_%_ext nimble_autoadv%
SUBMODS := $(filter-out $(IGNORE),$(filter nimble_%,$(USEMODULE)))

.PHONY: all

all: $(SUBMODS)

# glue code and RIOT port modules
nimble_riot_contrib:
	$(QQ)"$(MAKE)" -C $(TDIR)/contrib/

nimble_porting_nimble:
	$(QQ)"$(MAKE)" -C $(PDIR)/porting/nimble/src/ -f $(RIOTPKG)/$(PKG_NAME)/nimble.porting.mk MODULE=$@

nimble_npl_riot:
	$(QQ)"$(MAKE)" -C $(PDIR)/porting/npl/riot/src/ -f $(RIOTPKG)/$(PKG_NAME)/nimble.npl.mk MODULE=$@

# generic modules
nimble_transport:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/transport/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_transport_hci_h4:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/transport/common/hci_h4/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# host modules
nimble_host:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/src/ -f $(TDIR)/nimble.host.mk

nimble_host_util:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/util/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_host_store_ram:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/store/ram/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# service implementations
nimble_svc_gap:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/gap/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_svc_gatt:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/gatt/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_svc_ipss:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/ipss/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# controller specific modules
nimble_controller:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/controller/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_drivers_nrf5x:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/drivers/$(CPU_FAM)/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# additional, RIOT specific nimble modules
nimble_addr:
	$(QQ)"$(MAKE)" -C $(TDIR)/addr/

nimble_autoconn:
	$(QQ)"$(MAKE)" -C $(TDIR)/autoconn

nimble_netif:
	$(QQ)"$(MAKE)" -C $(TDIR)/netif/

nimble_rpble:
	$(QQ)"$(MAKE)" -C $(TDIR)/rpble/

nimble_scanlist:
	$(QQ)"$(MAKE)" -C $(TDIR)/scanlist

nimble_scanner:
	$(QQ)"$(MAKE)" -C $(TDIR)/scanner

nimble_statconn:
	$(QQ)"$(MAKE)" -C $(TDIR)/statconn
