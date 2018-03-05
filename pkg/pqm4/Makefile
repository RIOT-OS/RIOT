PKG_NAME=pqm4
PKG_URL=https://github.com/jowlo/pqm4.git
PKG_VERSION=riot-port
PKG_LICENSE=PD

.PHONY: all

all: git-download
	@cp $(RIOTBASE)/pkg/pqm4/src/* $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR) libs

include $(RIOTBASE)/pkg/pkg.mk
