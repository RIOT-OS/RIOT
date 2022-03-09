PKG_NAME = nrf_modem_fw
PKG_URL = https://www.nordicsemi.com/-/media/Software-and-other-downloads/Dev-Kits/nRF9160-DK/nRF9160-modem-FW/mfw_nrf9160_1.3.0.zip
PKG_VERSION = 1.3.0
PKG_EXT     = zip
PKG_LICENSE = Nordic-3-clause-BSD

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0

PROG_ARGS = -r -f NRF91 --verify

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

prepare:
	@mkdir $(PKG_SOURCE_DIR)

all: $(PKG_SOURCE_DIR)/mfw_nrf9160_1.3.0.zip flash_fw

$(PKG_SOURCE_DIR)/mfw_nrf9160_1.3.0.zip:
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)

flash_fw:
	$(Q)nrfjprog $(PROG_ARGS) --program $(PKG_SOURCE_DIR)/mfw_nrf9160_1.3.0.zip --sectorerase

distclean::
	rm -f $(PKG_SOURCE_DIR)/mfw_nrf9160_1.3.0.zip
