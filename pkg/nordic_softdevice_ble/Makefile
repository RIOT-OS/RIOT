PKG_NAME = nordic_softdevice_ble
PKG_VERSION = 3288530
PKG_FILE = nrf5_iot_sdk_$(PKG_VERSION).zip
PKG_URL = https://developer.nordicsemi.com/nRF5_IoT_SDK/nRF5_IoT_SDK_v0.9.x/$(PKG_FILE)
PKG_MD5 = 291c875df69c51e22491e6112128bd9c
PKG_LICENSE = nordic-bsd
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SRCDIR=$(PKG_BUILDDIR)/src

SOFTDEVICE := components/softdevice/s1xx_iot/s1xx-iot-prototype3_nrf52_softdevice.hex
BLE_6LOWPAN_LIB := components/iot/ble_6lowpan/lib/ble_6lowpan.a
MODULE_MAKEFILE := $(PKG_DIR)/Makefile.module

.PHONY: all prepare clean distclean

all: $(BINDIR)/ble_6lowpan.a $(BINDIR)/softdevice.hex

prepare: $(PKG_SRCDIR)/.extracted

$(BINDIR)/ble_6lowpan.a: $(PKG_SRCDIR)/.extracted
	cp $(PKG_SRCDIR)/$(BLE_6LOWPAN_LIB) $@

$(BINDIR)/softdevice.hex: $(PKG_SRCDIR)/.extracted
	cp $(PKG_SRCDIR)/$(SOFTDEVICE) $@

$(PKG_SRCDIR)/.extracted: $(PKG_BUILDDIR)/$(PKG_FILE)
	rm -rf $(@D)
	mkdir -p $(@D)
	$(Q)cd $(@D) && $(UNZIP_HERE) $(PKG_BUILDDIR)/$(PKG_FILE)

# this file doesn't compile with RIOT, but is not needed either
	rm $(PKG_BUILDDIR)/src/components/ble/common/ble_conn_params.c

# setup RIOT module makefiles for nordic SDK components
	cat $(MODULE_MAKEFILE) > $(PKG_SRCDIR)/components/softdevice/common/softdevice_handler/Makefile

	echo "MODULE=ble_common" > $(PKG_SRCDIR)/components/ble/common/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/components/ble/common/Makefile

	cat $(MODULE_MAKEFILE) > $(PKG_SRCDIR)/components/iot/ble_ipsp/Makefile

	touch $@

$(PKG_BUILDDIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	$(Q)$(DLCACHE) $(PKG_URL) $(PKG_MD5) $@

clean::
	rm -rf $(PKG_SRCDIR)/

distclean::
	rm -rf $(PKG_BUILDDIR)/
