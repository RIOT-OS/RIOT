PKG_NAME     = setsid
PKG_URL      = https://github.com/tzvetkoff/setsid-macosx
PKG_VERSION  = e5b851df41591021baf5cf88d4e41572baf8e08b
PKG_LICENSE  = BSD-2-Clause
PKG_BUILDDIR = $(CURDIR)/bin

include $(RIOTBASE)/pkg/pkg.mk

all:
	@echo "[INFO] compiling setsid from source now"
	$(MAKE) BINDIR=$(PKG_BUILDDIR) -C $(PKG_BUILDDIR)
	@mv $(PKG_BUILDDIR)/setsid $(CURDIR)/setsid

distclean::
	@rm -f $(CURDIR)/setsid
