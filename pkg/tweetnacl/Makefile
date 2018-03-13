PKG_NAME=tweetnacl
PKG_URL=https://github.com/kaspar030/tweetnacl
PKG_VERSION=416bec5f3abd8cdda94ddbc1c093e5243703aee4
PKG_LICENSE=PD

.PHONY: all

all: git-download
	@cp $(RIOTBASE)/pkg/tweetnacl/src/* $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
