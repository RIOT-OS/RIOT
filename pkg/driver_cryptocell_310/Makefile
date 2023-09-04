PKG_NAME=driver_cryptocell_310
PKG_URL=https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries
PKG_VERSION=17.1.0
PKG_EXT=zip
PKG_DIR_NAME=nRF5_SDK_17.1.0_ddde560
PKG_LICENSE=ARM Object Code and Header Files License

PKG_SOURCE_DIR ?= $(PKGDIRBASE)/$(PKG_NAME)
NRF_CC310_PATH = $(PKG_DIR_NAME)/external/nrf_cc310
PKG_ZIPFILE = $(PKG_DIR_NAME).$(PKG_EXT)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean distclean prepare

prepare: $(PKG_PREPARED)
	@:

all: $(PKGDIRBASE)/$(PKG_ZIPFILE)
	$(Q)$(UNZIP_HERE) -D -n -d $(PKGDIRBASE) $(PKGDIRBASE)/$(PKG_ZIPFILE)
	$(Q) mkdir -p $(PKG_SOURCE_DIR)/include
	$(Q)cp $(PKGDIRBASE)/$(NRF_CC310_PATH)/include/* $(PKG_SOURCE_DIR)/include
	$(Q)cp $(PKGDIRBASE)/$(NRF_CC310_PATH)/lib/cortex-m4/hard-float/libnrf_cc310_0.9.13.a $(PKG_SOURCE_DIR)
	$(Q)rm -rf $(PKGDIRBASE)/$(PKG_DIR_NAME)

$(PKGDIRBASE)/$(PKG_ZIPFILE):
	$(QQ)mkdir -p $(PKGDIRBASE)
	$(Q)$(DOWNLOAD_TO_FILE) $(PKGDIRBASE)/$(PKG_ZIPFILE) $(PKG_URL)/$(PKG_ZIPFILE)

clean::
	rm -rf $(PKG_SOURCE_DIR)

distclean::
	rm -rf $(PKG_SOURCE_DIR) $(PKGDIRBASE)/$(PKG_ZIPFILE)
