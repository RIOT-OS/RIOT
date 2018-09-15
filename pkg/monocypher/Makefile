PKG_NAME=monocypher
PKG_URL=https://github.com/LoupVaillant/Monocypher
PKG_VERSION=d9cc2aea29158971ed4b7dc074efdcb35e7183d5
PKG_LICENSE=CC-0

.PHONY: all

all: git-download
	$(Q)"$(MAKE)" -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
