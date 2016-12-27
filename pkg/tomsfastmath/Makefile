PKG_NAME = tomsfastmath
PKG_URL = https://github.com/libtom/tomsfastmath.git
PKG_VERSION = 5c395e04e6617f59ec9a57ef16352ee7c43a475b # v0.13.0

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)/src
include $(RIOTBASE)/pkg/pkg.mk
