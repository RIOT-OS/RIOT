PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
#PKG_VERSION=1261ea701c82b5e163be4a5551a228c730a47271
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
		$($(PKG_BUILDIR)/bootstrap)
		$($(PKG_BUILDIR)/configure --enabe-cli --host=$(TARGET_ARCH))
		$(MAKE) -C $(PKG_BUILDDIR)
		cp $(PKG_BUILDDIR)/src/core/libopenthread.a $(BINDIR)/$(PKG_NAME).a
		cp $(PKG_BUILDDIR)/third_party/mbedtls/libmbedcrypto.a $(BINDIR)/$(PKG_NAME)_crypto.a
		cp $(PKG_BUILDDIR)/src/cli/libopenthread-cli.a $(BINDIR)/$(PKG_NAME)_cli.a

include $(RIOTBASE)/pkg/pkg.mk
