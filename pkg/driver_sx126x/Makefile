PKG_NAME=driver_sx126x
PKG_URL=https://github.com/Lora-net/sx126x_driver
PKG_VERSION=ba61312213450ae94a4293d75285c1d8f30c04b3
PKG_LICENSE=BSD

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk
