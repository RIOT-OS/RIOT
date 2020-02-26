PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.1.4
PKG_VERSION=ce2c01f098f4d2b9479de5a796c3bb531f1fe14c
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs2
