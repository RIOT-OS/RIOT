PKG_NAME=qDSA
PKG_URL=https://github.com/RIOT-OS/qDSA.git
PKG_VERSION=dd2392b0c81ce4187fd3e1e2d3e0a4767f75782e
PKG_LICENSE=PD

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/$(QDSA_IMPL)

include $(RIOTBASE)/pkg/pkg.mk
