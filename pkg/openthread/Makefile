PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=31bc49de323ff953a72c826cddc14fffc7dccf31
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
		$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
