PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=e62b5b601bc1caa3e4d8172824988536ed6138f3
PKG_LICENSE=Apache-2.0

.PHONY: all

CFLAGS += -Wno-implicit-fallthrough

all: git-download
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
