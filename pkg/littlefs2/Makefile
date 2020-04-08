PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.2.0
PKG_VERSION=a049f1318eecbe502549f9d74a41951985fb956f
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs2
