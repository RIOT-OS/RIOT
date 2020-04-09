PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.2.1
PKG_VERSION=4c9146ea539f72749d6cc3ea076372a81b12cb11
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs2
