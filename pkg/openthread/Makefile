PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=31bc49de323ff953a72c826cddc14fffc7dccf31
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

$(info $(TARGET_ARCH))
$(info $(TARGET_ARCH))
$(info $(TARGET_ARCH))
$(info $(TARGET_ARCH))
$(info $(TARGET_ARCH))
$(info $(TARGET_ARCH))
all: git-download
		PREFIX=/ sh -c '$(MAKE) -C $(PKG_BUILDDIR)'
		cp $(PKG_BUILDDIR)/src/core/libopenthread.a $(BINDIR)/$(PKG_NAME).a
		cp $(PKG_BUILDDIR)/third_party/mbedtls/libmbedcrypto.a $(BINDIR)/$(PKG_NAME)_crypto.a
		cp $(PKG_BUILDDIR)/src/cli/libopenthread-cli.a $(BINDIR)/$(PKG_NAME)_cli.a

include $(RIOTBASE)/pkg/pkg.mk
