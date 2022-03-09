PKG_NAME=nrfxlib
PKG_URL=https://github.com/nrfconnect/sdk-nrfxlib
PKG_VERSION=9c49c779a2c938dd6a4a2e7192f4ff1d9c32f346
PKG_LICENSE=-Nordic-5-Clause

include $(RIOTBASE)/pkg/pkg.mk

ARCHIVE_PATH = $(PKG_SOURCE_DIR)/nrf_modem/lib/cortex-m33/soft-float/libmodem.a
PKG_BUILD_DIR = $(BUILD_DIR)/pkg/$(PKG_NAME)

NRFXLIB_MODULES := $(filter-out $(IGNORE_MODULES),$(filter nrfxlib_%,$(USEMODULE)))

.PHONY: nrfxlib_%

all: $(NRFXLIB_MODULES) $(BINDIR)/libmodem.a

# Need to sort out USEMODULE by name and remove nrfxlib_ from the name...
nrfxlib_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(strip $(subst nrfxlib_,, $@))/lib -f $(CURDIR)/Makefile.$(strip $(subst nrfxlib_,, $@))

$(BINDIR)/libmodem.a: $(ARCHIVE_PATH)
	@cp $< $@
