PKG_NAME=monocypher
PKG_URL=https://github.com/LoupVaillant/Monocypher
PKG_VERSION=6c2edbb48a61f43452c48c5f1e3ddabb19af3111
PKG_LICENSE=CC-0

.PHONY: all

all: git-download
	$(Q)"$(MAKE)" -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
