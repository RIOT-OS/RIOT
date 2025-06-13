PKG_NAME    = c25519
PKG_URL     = https://www.dlbeer.co.nz/downloads
PKG_VERSION = 2017-10-05
PKG_EXT     = zip
# The author of this package states that this package
# should be in public domain, which is legally not a license but more a
# state a historic work can reach, cf this statement:
# https://wiki.spdx.org/view/Legal_Team/Decisions/Dealing_with_Public_Domain_within_SPDX_Files
# The following license field is therefore set to empty.
PKG_LICENSE =
PKG_SHA512  = dbfb4285837ab2ea3d99c448b22877cc7a139ccbaebb1de367e2bec1fd562fe629b389d86603915448078b8fd7e631c8fc9a7d126eb889a1ba0c17611369b190

PKG_SOURCE_DIR ?= $(PKGDIRBASE)/$(PKG_NAME)
PKG_ZIPFILE = $(PKGDIRBASE)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
PKG_ZIP_URL = $(PKG_URL)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)

ifneq ($(RIOTBASE),)
include $(RIOTBASE)/Makefile.base
endif

.PHONY: all clean distclean

prepare: $(PKG_SOURCE_DIR)/

all: $(PKG_SOURCE_DIR)/
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

$(PKG_SOURCE_DIR)/: $(PKG_ZIPFILE)
	$(QQ)test "$(PKG_SHA512)  $(PKG_ZIPFILE)" =  "$$(sha512sum "$(PKG_ZIPFILE)")"
	$(Q)$(UNZIP_HERE) -D -d $(PKGDIRBASE) $<

$(PKG_ZIPFILE):
	$(QQ)mkdir -p $(PKGDIRBASE)
	$(Q)$(DOWNLOAD_TO_FILE) $@ $(PKG_ZIP_URL)

clean::
	# Reset package to checkout state.
	rm -rf $(PKG_SOURCE_DIR)

distclean::
	rm -rf $(PKG_SOURCE_DIR) $(PKG_ZIPFILE)
