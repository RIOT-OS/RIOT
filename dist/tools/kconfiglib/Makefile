PKG_NAME=kconfiglib
PKG_URL=https://github.com/ulfalizer/Kconfiglib
PKG_VERSION=82f59179b1b35fcd8b6d188453b283599ea70518
PKG_LICENSE=ISC
PKG_BUILDDIR=$(CURDIR)/bin

include $(RIOTBASE)/pkg/pkg.mk

all:
	cp $(PKG_BUILDDIR)/kconfiglib.py $(PKG_BUILDDIR)/menuconfig.py \
	   $(PKG_BUILDDIR)/genconfig.py $(PKG_BUILDDIR)/examples/merge_config.py \
	   .
