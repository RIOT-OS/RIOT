PKG_NAME=minmea
PKG_URL=https://github.com/kosma/minmea
PKG_VERSION=ae4dd9442a9041345d5ef108f062e7e4ec6954f2
PKG_LICENSE=WTFPL

include $(RIOTBASE)/pkg/pkg.mk

all:
	@cp Makefile.$(PKG_NAME) $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
