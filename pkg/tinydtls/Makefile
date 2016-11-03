PKG_NAME=tinydtls
PKG_URL=git://github.com/rfuentess/TinyDTLS.git
PKG_VERSION=RIOT-Tested-Upgrades
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
