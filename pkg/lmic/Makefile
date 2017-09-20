PKG_NAME=lmic
PKG_URL=https://github.com/matthijskooijman/arduino-lmic
PKG_VERSION=9eae2eb8155215f1ff573034d0341f4e5be57bfe

PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
