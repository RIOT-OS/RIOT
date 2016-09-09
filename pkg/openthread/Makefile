PKG_NAME=openthread
PKG_URL=git://github.com/openthread/openthread.git
PKG_VERSION=c05c632d2f8453200dd5be15a975e71f340cafca
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
		$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
