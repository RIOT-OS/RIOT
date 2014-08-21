PKG_NAME=RB
PKG_URL=https://codeload.github.com/openwsn-berkeley/openwsn-fw
PKG_VERSION=1.4
PKG_EXT=zip

.PHONY: all clean

all: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/
	$(info *INFO* Building OpenWSN pkg)
	make -C $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	$(AD)rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)
	$(AD)$(UNZIP_HERE) $< -d $(PKG_NAME)-$(PKG_VERSION)
	$(AD)cd $@ && sh $(CURDIR)/structure_changes.sh
	$(AD)cd $@ && sh $(CURDIR)/apply_patches.sh

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT):
	# Get PKG_VERSION of package from PKG_URL
	$(AD)$(DOWNLOAD_TO_FILE) $@ $(PKG_URL)/$(PKG_EXT)/$(PKG_NAME)-$(PKG_VERSION)

clean::
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)

distclean::
	rm -f $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
