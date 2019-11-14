PKG_NAME    = nimble
PKG_URL     = https://github.com/apache/mynewt-nimble.git
PKG_VERSION = fb7651b773734a075c47ff10210b92e6d55ef55b
PKG_LICENSE = Apache-2.0

TDIR = $(RIOTPKG)/$(PKG_NAME)
PDIR = $(PKG_BUILDDIR)

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

# blue code and RIOT port modules
nimble_riot_contrib:
	"$(MAKE)" -C $(TDIR)/contrib/

nimble_porting_nimble:
	"$(MAKE)" -C $(PDIR)/porting/nimble/src/ -f $(TDIR)/porting.nimble.mk

nimble_npl_riot:
	"$(MAKE)" -C $(PDIR)/porting/npl/riot/src/ -f $(TDIR)/porting.npl.riot.mk

# host modules
nimble_host:
	"$(MAKE)" -C $(PDIR)/nimble/host/src/ -f $(TDIR)/nimble.host.mk

nimble_host_util:
	"$(MAKE)" -C $(PDIR)/nimble/host/util/src/ -f $(TDIR)/nimble.host.util.mk

nimble_host_store_ram:
	"$(MAKE)" -C $(PDIR)/nimble/host/store/ram/src/ -f $(TDIR)/nimble.host.store.ram.mk

nimble_tinycrypt:
	"$(MAKE)" -C $(PDIR)/ext/tinycrypt/src/ -f $(TDIR)/ext.tinycrypt.mk

# service implementations
nimble_svc_gap:
	"$(MAKE)" -C $(PDIR)/nimble/host/services/gap/src/ -f $(TDIR)/svc.gap.mk

nimble_svc_gatt:
	"$(MAKE)" -C $(PDIR)/nimble/host/services/gatt/src/ -f $(TDIR)/svc.gatt.mk

nimble_svc_ipss:
	"$(MAKE)" -C $(PDIR)/nimble/host/services/ipss/src/ -f $(TDIR)/svc.ipss.mk

# controller specific modules
nimble_transport_ram:
	"$(MAKE)" -C $(PDIR)/nimble/transport/ram/src/ -f $(TDIR)/transport.ram.mk

nimble_controller:
	"$(MAKE)" -C $(PDIR)/nimble/controller/src/ -f $(TDIR)/controller.mk

nimble_drivers_nrf5x:
	"$(MAKE)" -C $(PDIR)/nimble/drivers/$(CPU_FAM)/src/ -f $(TDIR)/drivers.nrf5x.mk

# additional, RIOT specific nimble modules
nimble_addr:
	"$(MAKE)" -C $(TDIR)/addr/

nimble_autoconn:
	"$(MAKE)" -C $(TDIR)/autoconn

nimble_netif:
	"$(MAKE)" -C $(TDIR)/netif/

nimble_scanlist:
	"$(MAKE)" -C $(TDIR)/scanlist

nimble_scanner:
	"$(MAKE)" -C $(TDIR)/scanner

include $(RIOTBASE)/pkg/pkg.mk
