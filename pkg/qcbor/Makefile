PKG_NAME    = qcbor
PKG_URL     = https://github.com/laurencelundblade/QCBOR
PKG_VERSION = ee146ab6105344a18744b34c38bc1cf3e5135b02 # Commit on Feb 19, 2020 
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(RIOTPKG)/qcbor/Makefile.$(PKG_NAME)
