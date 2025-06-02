PKG_NAME=esp32_sdk_mbedtls
PKG_URL=https://github.com/espressif/mbedtls.git
PKG_VERSION=98fcfd6d2cea90d306e8fde8e5bffd6087c9cda8
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
