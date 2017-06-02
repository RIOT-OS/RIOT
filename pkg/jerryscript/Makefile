PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=56802c22a10b0d684ac93921ac28df891e320b4a
PKG_LICENSE=Apache-2.0

.PHONY: all

all: git-download
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
