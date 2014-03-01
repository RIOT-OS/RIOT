PKG_NAME=RB
PKG_URL=https://codeload.github.com/openwsn-berkeley/openwsn-fw
PKG_VERSION=1.4
PKG_EXT=zip

FETCH=$(shell which wget &> /dev/null && echo "wget" || echo "curl")
#UNPACK=tar -xvf
UNPACK=unzip

ifeq ($(FETCH),curl )
	FETCH_FLAGS += -z $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT) -o
else
	FETCH_FLAGS += -nc -O
endif

ifneq ($(RIOTBOARD),)
#include $(RIOTBOARD)/Makefile.base
include $(RIOTBOARD)/$(BOARD)/Makefile.include
endif

.PHONY: all clean

all: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/
	make -C $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)/: $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
	$(AD)rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)
	$(AD)$(UNPACK) $< -d $(PKG_NAME)-$(PKG_VERSION)
	$(AD)cd $@ && sh ../structure_changes.sh
	$(AD)cd $@ && patch -p1 -N -i ../patch.txt

$(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT):
	# Get PKG_VERSION of package from PKG_URL
	$(AD)$(FETCH) $(FETCH_FLAGS) $@ $(PKG_URL)/$(PKG_EXT)/$(PKG_NAME)-$(PKG_VERSION) || true

clean::
	rm -rf $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION)

distclean::
	rm -f $(CURDIR)/$(PKG_NAME)-$(PKG_VERSION).$(PKG_EXT)
