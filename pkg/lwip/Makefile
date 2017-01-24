PKG_NAME=lwip
PKG_URL=git://git.savannah.nongnu.org/lwip.git
PKG_VERSION=fd4a109ffa6513b28a0c780a952cef1110423717
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
