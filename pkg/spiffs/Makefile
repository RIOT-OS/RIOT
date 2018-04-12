PKG_NAME=spiffs
PKG_URL=https://github.com/pellepl/spiffs.git
PKG_VERSION=287148c46587089c4543a21eef2d6e9e14b88364
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.spiffs

include $(RIOTBASE)/pkg/pkg.mk
