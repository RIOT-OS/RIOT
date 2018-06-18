PKG_NAME = tiny-asn1
PKG_URL = https://gitlab.com/mtausig/tiny-asn1.git
PKG_VERSION = 82e3a26273900b532e33e5b377f193fa08ee7d1b
PKG_LICENSE = LGPL-3

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
