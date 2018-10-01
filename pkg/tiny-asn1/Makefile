PKG_NAME = tiny-asn1
PKG_URL = https://gitlab.com/mtausig/tiny-asn1.git
PKG_VERSION = 5155ee9c8f0908af955ca2026c349ba8a5e12bc2
PKG_LICENSE = LGPL-3

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
