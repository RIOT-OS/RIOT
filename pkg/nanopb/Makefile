PKG_NAME=nanopb
PKG_URL=https://github.com/nanopb/nanopb
PKG_VERSION=493adf3616bee052649c63c473f8355630c2797f # nanopb-0.3.9.4
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.nanopb

include $(RIOTBASE)/pkg/pkg.mk
