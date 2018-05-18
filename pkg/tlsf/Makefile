PKG_NAME=tlsf
PKG_URL=https://github.com/mattconte/tlsf
PKG_VERSION=a1f743ffac0305408b39e791e0ffb45f6d9bc777
PKG_LICENSE=BSD

.PHONY: all

all: Makefile.tlsf
	@cp Makefile.tlsf $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)


include $(RIOTBASE)/pkg/pkg.mk
