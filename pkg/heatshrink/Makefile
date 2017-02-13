PKG_NAME=heatshrink
PKG_URL=https://github.com/atomicobject/heatshrink.git
PKG_VERSION=7d419e1fa4830d0b919b9b6a91fe2fb786cf3280

.PHONY: all

all: git-download
	cp Makefile.heatshrink $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
