PKG_NAME=heatshrink
PKG_URL=https://github.com/atomicobject/heatshrink.git
PKG_VERSION=7d419e1fa4830d0b919b9b6a91fe2fb786cf3280

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.heatshrink

include $(RIOTBASE)/pkg/pkg.mk
