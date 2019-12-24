PKG_NAME=littlefs
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v1.7.2
PKG_VERSION=7e110b44c0e796dc56e2fe86587762d685653029
PKG_LICENSE=Apache-2.0

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs

include $(RIOTBASE)/pkg/pkg.mk
