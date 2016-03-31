PKG_NAME=emb6
PKG_URL=https://github.com/hso-esk/emb6.git
PKG_VERSION=14e4a3cfff01644e078870e14e16a1fe60dcc895
PKG_BUILDDIR ?= $(BINDIRBASE)/pkg/$(BOARD)/$(PKG_NAME)

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
