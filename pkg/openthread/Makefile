PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=d3e20adafc01d78c1a47f9dcbc8f69de71c752cf
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
		PREFIX=/ sh -c '$(MAKE) -C $(PKG_BUILDDIR)'
		cp $(PKG_BUILDDIR)/src/core/libopenthread.a $(BINDIR)/$(PKG_NAME).a
		cp $(PKG_BUILDDIR)/third_party/mbedtls/libmbedcrypto.a $(BINDIR)/$(PKG_NAME)_crypto.a
		cp $(PKG_BUILDDIR)/src/cli/libopenthread-cli.a $(BINDIR)/$(PKG_NAME)_cli.a

include $(RIOTBASE)/pkg/pkg.mk
