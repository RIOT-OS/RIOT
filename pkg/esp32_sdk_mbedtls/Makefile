PKG_NAME=esp32_sdk_mbedtls
PKG_URL=https://github.com/espressif/mbedtls.git
PKG_VERSION=a2b32072ea898afc1ed5b6caf6931e36028c91d6
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
