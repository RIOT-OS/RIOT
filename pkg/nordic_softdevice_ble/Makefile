PKG_NAME = nordic_softdevice_ble
PKG_VERSION = 3288530
PKG_FILE = nrf5_iot_sdk_$(PKG_VERSION).zip
PKG_URL = https://developer.nordicsemi.com/nRF5_IoT_SDK/nRF5_IoT_SDK_v0.9.x/$(PKG_FILE)
PKG_MD5 = 291c875df69c51e22491e6112128bd9c
PKG_LICENSE = nordic-bsd

PKG_DIR = $(CURDIR)
PKG_BUILD_DIR = $(BINDIR)/pkg/$(PKG_NAME)
PKG_SOURCE_DIR = $(PKGDIRBASE)/$(PKG_NAME)
PKG_SRCDIR  = $(PKG_SOURCE_DIR)/src

SOFTDEVICE := components/softdevice/s1xx_iot/s1xx-iot-prototype3_nrf52_softdevice.hex
BLE_6LOWPAN_LIB := components/iot/ble_6lowpan/lib/ble_6lowpan.a

SOFTDEVICE_MODULES = $(filter softdevice_handler ble_common ble_ipsp,$(USEMODULE))

.PHONY: all prepare clean distclean $(SOFTDEVICE_MODULES)

all: $(BINDIR)/ble_6lowpan.a $(BINDIR)/softdevice.hex $(SOFTDEVICE_MODULES)

prepare: $(PKG_SRCDIR)/.extracted

softdevice_handler:
	"$(MAKE)" -C $(PKG_SRCDIR)/components/softdevice/common/softdevice_handler -f $(RIOTBASE)/Makefile.base

ble_common:
	"$(MAKE)" -C $(PKG_SRCDIR)/components/ble/common -f $(CURDIR)/ble_common.mk

ble_ipsp:
	"$(MAKE)" -C $(PKG_SRCDIR)/components/iot/ble_ipsp -f $(RIOTBASE)/Makefile.base

$(BINDIR)/ble_6lowpan.a: $(PKG_SRCDIR)/.extracted
	cp $(PKG_SRCDIR)/$(BLE_6LOWPAN_LIB) $@

# softdevice.hex has the `[0x8bc, 0x3000[` addresses not set.
# However, it requires that the value at `0x2000` is 0xFFFFFFFF
# We just put all the undefined memory to 0xff as it is the rom reset value anyway.
$(BINDIR)/softdevice.hex: $(PKG_SRCDIR)/.extracted
	$(Q)$(OBJCOPY) $(OFLAGS) -Oihex --gap-fill 0xff $(PKG_SRCDIR)/$(SOFTDEVICE) $@

$(PKG_SRCDIR)/.extracted: $(PKG_SOURCE_DIR)/$(PKG_FILE)
	rm -rf $(@D)
	mkdir -p $(@D)
	$(Q)cd $(@D) && $(UNZIP_HERE) $(PKG_SOURCE_DIR)/$(PKG_FILE)
	touch $@

$(PKG_SOURCE_DIR)/$(PKG_FILE):
	@mkdir -p $(@D)
	$(Q)$(DLCACHE) $(PKG_URL) $(PKG_MD5) $@

clean::
	rm -rf $(PKG_SOURCE_DIR)/

distclean::
	rm -rf $(PKG_BUILD_DIR)/
