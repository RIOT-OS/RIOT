PKG_NAME = tiny-asn1
PKG_URL = https://gitlab.com/mtausig/tiny-asn1.git
PKG_VERSION = 5155ee9c8f0908af955ca2026c349ba8a5e12bc2
PKG_LICENSE = LGPL-3

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
