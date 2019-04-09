PKG_NAME=lsyslogd
PKG_URL=https://github.com/crest42/lsyslogd.git
PKG_VERSION=8e18b4ed39e2d81ee8515253951e516e4cb9b7f7
MODULE_MAKEFILE := $(CURDIR)/Makefile.lsyslogd

.PHONY: all

all: git-download
		@cp $(MODULE_MAKEFILE) $(PKG_BUILDDIR)/Makefile
		@cp $(PKG_BUILDDIR)/src/lsyslogd.c $(PKG_BUILDDIR)/
		@cp $(PKG_BUILDDIR)/src/outputs/*.c $(PKG_BUILDDIR)/

		"$(MAKE)" -C $(PKG_BUILDDIR)


include $(RIOTBASE)/pkg/pkg.mk
