PKG_NAME=tinyusb
PKG_URL=https://github.com/hathach/tinyusb
# TinyUSB 0.15.0
PKG_VERSION=86c416d4c0fb38432460b3e11b08b9de76941bf5

PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

PSRC = $(PKG_SOURCE_DIR)/src
PLIB = $(PKG_SOURCE_DIR)/lib

.PHONY: all

all: $(filter tinyusb_% stdio_tinyusb_cdc_acm,$(filter-out tinyusb_class_net_%,$(USEMODULE)))
	$(QQ)"$(MAKE)" -C $(PSRC) -f $(RIOTBASE)/Makefile.base MODULE=tinyusb

stdio_tinyusb_cdc_acm:
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/$(PKG_NAME)/cdc_acm_stdio

tinyusb_contrib:
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/$(PKG_NAME)/contrib

tinyusb_dfu:
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/$(PKG_NAME)/dfu

tinyusb_hw:
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/$(PKG_NAME)/hw

tinyusb_netdev:
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/$(PKG_NAME)/netdev

tinyusb_class_audio:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/audio -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_bth:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/bth -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_cdc:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/cdc -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_dfu:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/dfu -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_dfu_runtime:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/dfu -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_hid:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/hid -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_midi:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/midi -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_msc:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/msc -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_net:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/net -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_usbtmc:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/usbtmc -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_vendor:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/vendor -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_class_video:
	$(QQ)"$(MAKE)" -C $(PSRC)/class/video -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_common:
	$(QQ)"$(MAKE)" -C $(PSRC)/common -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_device:
	$(QQ)"$(MAKE)" -C $(PSRC)/device -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_host:
	$(QQ)"$(MAKE)" -C $(PSRC)/host -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_lib_networking:
	$(QQ)"$(MAKE)" -C $(PLIB)/networking -f $(RIOTPKG)/$(PKG_NAME)/Makefile.lib_networking

tinyusb_portable_espressif:
	$(QQ)"$(MAKE)" -C $(PSRC)/portable/espressif/esp32sx -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_portable_microchip:
	$(QQ)"$(MAKE)" -C $(PSRC)/portable/microchip/samd -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_portable_nrf5x:
	$(QQ)"$(MAKE)" -C $(PSRC)/portable/nordic/nrf5x -f $(RIOTPKG)/$(PKG_NAME)/Makefile.nrf52

tinyusb_portable_stm32_fsdev:
	$(QQ)"$(MAKE)" -C $(PSRC)/portable/st/stm32_fsdev -f $(RIOTBASE)/Makefile.base MODULE=$@

tinyusb_portable_synopsys_dwc2:
	$(QQ)"$(MAKE)" -C $(PSRC)/portable/synopsys/dwc2 -f $(RIOTBASE)/Makefile.base MODULE=$@
