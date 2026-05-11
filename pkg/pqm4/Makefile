PKG_NAME    = pqm4
PKG_URL     = https://github.com/mupq/pqm4
# pqm4 master, 2025-05-22 (Merge PR #392 - Update uov to round2)
PKG_VERSION = a24bb4b662016968c19f5e6a0719c9ad530f0286
PKG_LICENSE = CC0-1.0

include $(RIOTBASE)/pkg/pkg.mk

# Build the pqm4 ML-DSA-44 m4f module via our custom Makefile.pqm4.
all: prepare
	"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.pqm4

# pqm4 has git submodules (mupq + nested pqclean) that pkg.mk does not init.
# Hook submodule init before $(PKG_PATCHED) so patches can target submodule
# contents if ever needed.
SUBMODULES_DONE := $(PKG_SOURCE_DIR)/.submodules-initialized

$(PKG_PATCHED): $(SUBMODULES_DONE)

$(SUBMODULES_DONE): $(PKG_DOWNLOADED)
	cd $(PKG_SOURCE_DIR) && git submodule update --init --recursive mupq
	touch $@
