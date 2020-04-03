PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=bb8e51f6da3e3e3fc65a8b9abaea1b0ebbbe104d
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

include $(RIOTBASE)/pkg/pkg.mk

all:
	@mkdir -p $(PKG_BUILDDIR)/tmp
	BUILD=$(PKG_BUILDDIR) "$(MAKE)" -C $(PKG_BUILDDIR)/ports/riot
