PKG_NAME = tiny-asn1
PKG_URL = https://gitlab.com/matthegap/tiny-asn1.git
PKG_VERSION = b09f058966c6296904487c3f8fc04c68fe83b2cc
PKG_LICENSE = LGPL-3

export TINYASN1_ROOT=$(CURDIR)
INCLUDES+=-I$(TINYASN1_ROOT)/src

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
