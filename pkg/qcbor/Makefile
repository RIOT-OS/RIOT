PKG_NAME    = qcbor
PKG_URL     = https://github.com/laurencelundblade/QCBOR
PKG_VERSION = 7e8effa94174c33e4acaf737adf6aef16c092c49 # Commit on Mar 9, 2020
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
