PKG_NAME=minmea
PKG_URL=https://github.com/kosma/minmea
PKG_VERSION=dbf51f03c5058746b4c4b3d6fdcd3d0357c5da36
PKG_LICENSE=WTFPL

.PHONY: all

all: git-download
	@cp Makefile.$(PKG_NAME) $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
