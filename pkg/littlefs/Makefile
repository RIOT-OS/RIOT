PKG_NAME=littlefs
PKG_URL=https://github.com/geky/littlefs.git
PKG_VERSION=v1.3
PKG_LICENSE=Apache-2.0

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs

include $(RIOTBASE)/pkg/pkg.mk
