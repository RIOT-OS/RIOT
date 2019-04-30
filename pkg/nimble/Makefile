PKG_NAME    = nimble
PKG_URL     = https://github.com/apache/mynewt-nimble.git
PKG_VERSION = a7b3c939146e735b59d55bff740c906bde6f86f9
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
else
  CFLAGS += -Wno-unused-but-set-variable
endif

SUBMODS := $(filter nimble_%,$(USEMODULE))

.PHONY: all

all: git-download $(SUBMODS)

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

# controller specific modules
nimble_transport_ram:
	"$(MAKE)" -C $(PDIR)/nimble/transport/ram/src/ -f $(TDIR)/transport.ram.mk

nimble_controller:
	"$(MAKE)" -C $(PDIR)/nimble/controller/src/ -f $(TDIR)/controller.mk

nimble_drivers_nrf52:
	"$(MAKE)" -C $(PDIR)/nimble/drivers/nrf52/src/ -f $(TDIR)/drivers.nrf52.mk

# additional, RIOT specific nimble modules
nimble_addr:
	"$(MAKE)" -C $(TDIR)/addr/

nimble_scanlist:
	"$(MAKE)" -C $(TDIR)/scanlist

nimble_scanner:
	"$(MAKE)" -C $(TDIR)/scanner

include $(RIOTBASE)/pkg/pkg.mk
