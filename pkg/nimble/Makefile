PKG_NAME    = nimble
PKG_URL     = https://github.com/apache/mynewt-nimble.git
PKG_VERSION = 050a7108cf56546554b4162f0600ab3b2d7e173f
PKG_LICENSE = Apache-2.0

TDIR = $(RIOTPKG)/$(PKG_NAME)
PDIR = $(PKG_BUILDDIR)

# As of now, NimBLE does not build without warnings for -Wextra, so we disable
# that flag for this package. Will hopefully be fixed some time in the future.
CFLAGS += -Wno-extra
# the static function `ble_ll_adv_active_chanset_is_sec()` in
# `nimble/controller/src/ble_ll_adv.c` isn't used in our compilation path, so
# tell LLVM/clang not to be so pedantic with this.
ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-unused-function
endif

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PDIR)/ext/tinycrypt/src/ -f $(TDIR)/ext.tinycrypt.mk
	"$(MAKE)" -C $(PDIR)/nimble/src -f $(TDIR)/nimble.mk
	"$(MAKE)" -C $(PDIR)/nimble/host/src/ -f $(TDIR)/nimble.host.mk
	"$(MAKE)" -C $(PDIR)/nimble/host/services/gap/src/ -f $(TDIR)/service.gap.mk
	"$(MAKE)" -C $(PDIR)/nimble/host/services/gatt/src/ -f $(TDIR)/service.gatt.mk
	"$(MAKE)" -C $(PDIR)/nimble/host/util/src/ -f $(TDIR)/nimble.host.util.mk
	"$(MAKE)" -C $(PDIR)/nimble/host/store/ram/src/ -f $(TDIR)/nimble.host.store.ram.mk
	"$(MAKE)" -C $(PDIR)/porting/nimble/src/ -f $(TDIR)/porting.nimble.mk
	"$(MAKE)" -C $(PDIR)/porting/npl/riot/src/ -f $(TDIR)/porting.npl.riot.mk

	"$(MAKE)" -C $(PDIR)/nimble/transport/ram/src/ -f $(TDIR)/transport.ram.mk
	"$(MAKE)" -C $(PDIR)/nimble/controller/src/ -f $(TDIR)/controller.mk
	"$(MAKE)" -C $(PDIR)/nimble/drivers/nrf52/src/ -f $(TDIR)/drivers.nrf52.mk

	"$(MAKE)" -C $(TDIR)/contrib/

include $(RIOTBASE)/pkg/pkg.mk
