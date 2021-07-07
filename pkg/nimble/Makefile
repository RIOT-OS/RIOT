PKG_NAME    = nimble
PKG_URL     = https://github.com/apache/mynewt-nimble.git
PKG_VERSION = 940cc0b4a81295305ceead15928da24fe6e6f603
PKG_LICENSE = Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

TDIR = $(RIOTPKG)/$(PKG_NAME)
PDIR = $(PKG_SOURCE_DIR)

# NimBLE is not optimized for building with all (extra) warnings enabled. So for
# now, we disable a number of selected compiler warnings when building NimBLE.
CFLAGS += -Wno-extra
ifeq (llvm,$(TOOLCHAIN))
# the static function `ble_ll_adv_active_chanset_is_sec()` in
# `nimble/controller/src/ble_ll_adv.c` isn't used in our compilation path, so
# tell LLVM/clang not to be so pedantic with this.
  CFLAGS += -Wno-unused-function
# Workaround, until https://github.com/apache/mynewt-nimble/pull/566 is merged
# upstream and the NimBLE version in RIOT is updated.
  CFLAGS += -Wno-sometimes-uninitialized
else
  CFLAGS += -Wno-unused-but-set-variable
endif

IGNORE := nimble_autoconn_%
SUBMODS := $(filter-out $(IGNORE),$(filter nimble_%,$(USEMODULE)))

.PHONY: all

all: $(SUBMODS)

# glue code and RIOT port modules
nimble_riot_contrib:
	$(QQ)"$(MAKE)" -C $(TDIR)/contrib/

nimble_porting_nimble:
	$(QQ)"$(MAKE)" -C $(PDIR)/porting/nimble/src/ -f $(RIOTPKG)/$(PKG_NAME)/nimble.porting.mk MODULE=$@

nimble_npl_riot:
	$(QQ)"$(MAKE)" -C $(PDIR)/porting/npl/riot/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# host modules
nimble_host:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/src/ -f $(TDIR)/nimble.host.mk

nimble_host_util:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/util/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_host_store_ram:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/store/ram/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_tinycrypt:
	$(QQ)"$(MAKE)" -C $(PDIR)/ext/tinycrypt/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# service implementations
nimble_svc_gap:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/gap/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_svc_gatt:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/gatt/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_svc_ipss:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/host/services/ipss/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# controller specific modules
nimble_transport_ram:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/transport/ram/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_controller:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/controller/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

nimble_drivers_nrf5x:
	$(QQ)"$(MAKE)" -C $(PDIR)/nimble/drivers/$(CPU_FAM)/src/ -f $(RIOTBASE)/Makefile.base MODULE=$@

# additional, RIOT specific nimble modules
nimble_addr:
	$(QQ)"$(MAKE)" -C $(TDIR)/addr/

nimble_autoadv:
	$(QQ)"$(MAKE)" -C $(TDIR)/autoadv

nimble_autoconn:
	$(QQ)"$(MAKE)" -C $(TDIR)/autoconn

nimble_netif:
	$(QQ)"$(MAKE)" -C $(TDIR)/netif/

nimble_scanlist:
	$(QQ)"$(MAKE)" -C $(TDIR)/scanlist

nimble_scanner:
	$(QQ)"$(MAKE)" -C $(TDIR)/scanner

nimble_statconn:
	$(QQ)"$(MAKE)" -C $(TDIR)/statconn
