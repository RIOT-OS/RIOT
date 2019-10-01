PKG_NAME=littlefs
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.1.3
PKG_VERSION=6b65737715039ef92d348014316b575b52547019
PKG_LICENSE=Apache-2.0

.PHONY: all

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs

include $(RIOTBASE)/pkg/pkg.mk
