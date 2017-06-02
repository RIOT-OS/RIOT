PKG_NAME=lwip
PKG_URL=git://git.savannah.nongnu.org/lwip.git
PKG_VERSION=STABLE-2_0_2_RELEASE_VER
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
