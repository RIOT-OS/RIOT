PKG_NAME=UF2
PKG_URL=https://github.com/microsoft/uf2.git
PKG_VERSION=b19ac9ad613e11a062e4d00845e2b80e6b0ad29f
PKG_LICENSE=MIT

RIOTBASE ?= $(CURDIR)/../../..

PKG_SPARSE_PATHS  = utils/uf2conv.py
PKG_SPARSE_PATHS += utils/uf2families.json

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/uf2conv.py $(CURDIR)/uf2families.json

$(CURDIR)/uf2conv.py: $(PKG_PREPARED)
	@cp $(PKG_SOURCE_DIR)/utils/uf2conv.py $@
	@chmod a+x $@

$(CURDIR)/uf2families.json: $(PKG_PREPARED)
	@cp $(PKG_SOURCE_DIR)/utils/uf2families.json $@

clean::
	@rm -rf $(CURDIR)/uf2conv.py
	@rm -rf $(CURDIR)/uf2families.json
