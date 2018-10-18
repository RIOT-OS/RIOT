PKG_NAME=littlefs
PKG_URL=https://github.com/geky/littlefs.git
# v1.6.2
PKG_VERSION=0bb1f7af17755bd792f0c4966877fb1886dfc802
PKG_LICENSE=Apache-2.0

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.littlefs

include $(RIOTBASE)/pkg/pkg.mk
