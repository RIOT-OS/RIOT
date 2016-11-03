PKG_NAME=tinydtls
PKG_URL=git://github.com/rfuentess/TinyDTLS.git
# PKG_VERSION=RIOT-OS
PKG_VERSION=d06ad84d3e8bd3e86f7557faee34b68d5f8c0129
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
