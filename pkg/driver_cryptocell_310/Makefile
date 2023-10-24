PKG_NAME      = driver_cryptocell_310
PKG_URL       = https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries
PKG_VERSION   = 17.1.0_ddde560
PKG_DIR_NAME  = nRF5_SDK
PKG_EXT       = zip
PKG_LICENSE   = ARM Object Code and Header Files License
PKG_SHA512  	= cb610d724d27ef79c792511d8b889cbf2d411a3a391f5c740ea10b86139201b97eedf0c1edcf4d199c3fbd51e4cc5a4bd922c0c40d01e2f5359acd16a2b163e4

PKG_SOURCE_DIR ?= $(PKGDIRBASE)/$(PKG_NAME)
PKG_ZIP_DIR = $(PKGDIRBASE)/$(PKG_DIR_NAME)_$(PKG_VERSION)
PKG_ZIPFILE = $(PKGDIRBASE)/$(PKG_DIR_NAME)_$(PKG_VERSION).$(PKG_EXT)
PKG_ZIP_URL = $(PKG_URL)/$(PKG_DIR_NAME)_$(PKG_VERSION).$(PKG_EXT)

NRF_CC310_PATH = external/nrf_cc310

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean distclean

prepare: $(PKG_SOURCE_DIR)/

all: $(PKG_SOURCE_DIR)/

$(PKG_SOURCE_DIR)/: $(PKG_ZIPFILE)
	$(QQ)test "$(PKG_SHA512)  $(PKG_ZIPFILE)" =  "$$(sha512sum "$(PKG_ZIPFILE)")"
	$(Q)$(UNZIP_HERE) -D -d $(PKGDIRBASE) $<
	$(Q)mkdir -p $(PKG_SOURCE_DIR)/include
	$(Q)cp $(PKG_ZIP_DIR)/$(NRF_CC310_PATH)/include/* $(PKG_SOURCE_DIR)/include
	$(Q)cp $(PKG_ZIP_DIR)/$(NRF_CC310_PATH)/lib/cortex-m4/hard-float/libnrf_cc310_0.9.13.a $(PKG_SOURCE_DIR)
	$(Q)rm -rf $(PKG_ZIP_DIR)

$(PKG_ZIPFILE):
	$(QQ)mkdir -p $(PKGDIRBASE)
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_ZIP_URL)

clean::
	rm -rf $(PKG_SOURCE_DIR)

distclean::
	rm -rf $(PKG_SOURCE_DIR) $(PKG_ZIPFILE)
