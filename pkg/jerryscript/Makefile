PKG_NAME=jerryscript
PKG_URL=https://github.com/jerryscript-project/jerryscript.git
PKG_VERSION=b140158104064ef082ddb58052f8c7c7cee9675c
PKG_LICENSE=Apache-2.0

.PHONY: all

all: git-download
	@cp Makefile.jerryscript $(PKG_BUILDDIR)/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)
	cp $(PKG_BUILDDIR)/riot/lib/libjerry-core.a ${BINDIR}/libjerrycore.a

include $(RIOTBASE)/pkg/pkg.mk
