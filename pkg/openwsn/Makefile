PKG_NAME=RB
PKG_URL=https://github.com/openwsn-berkeley/openwsn-fw/archive
PKG_VERSION=1.4
PKG_EXT=zip

FETCH=$(shell which wget &> /dev/null && echo "wget" || echo "curl")
#UNPACK=tar -xvf
UNPACK=unzip

ifneq ($(RIOTBOARD),)
#include $(RIOTBOARD)/Makefile.base
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

.PHONY: all clean patch reset

all: patch
	make -C $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)

patch: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/Makefile
	# Dependancy might be changed accordingly though we think the Makefile
	# will be the first thing you want to change
	#
	# Here might not happen anything besides dependancy checks

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/Makefile: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/
	# Here you apply your patch.
	cd $< && sh ../structure_changes.sh
	cd $< && patch -p0 -N -i ../patch.txt

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)
	$(UNPACK) $< -d $(PKG_NAME)-$(PKG_VERSION)

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT):
	# rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)
	# Get PKG_VERSION of package from PKG_URL
	@$(FETCH) -O $@ $(PKG_URL)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT) || true

clean::
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION) $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
