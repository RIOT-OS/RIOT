PKG_NAME=esp32_sdk_eth_phy
PKG_URL=https://github.com/espressif/esp-eth-drivers
# dp83848, ip101, ksz80xx, lan87xx and rtl8201 v1.1.0
PKG_VERSION=4f1641dc8f1f0fe1e97af1025c4588e855b4ac25
PKG_LICENSE=Apache-2.0

# Only check out the Ethernet PHY drivers that were part of ESP-IDF before v6.0
PKG_SPARSE_PATHS += dp83848
PKG_SPARSE_PATHS += ip101
PKG_SPARSE_PATHS += ksz80xx
PKG_SPARSE_PATHS += lan87xx
PKG_SPARSE_PATHS += rtl8201

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
