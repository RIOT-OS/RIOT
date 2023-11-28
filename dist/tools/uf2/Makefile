PKG_NAME=UF2
PKG_URL=https://github.com/microsoft/uf2.git
PKG_VERSION=27e322fcdcc6eee0642242638d4f2557efb32559
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/uf2conv.py $(CURDIR)/uf2families.json

$(CURDIR)/uf2conv.py:
	cp $(PKG_SOURCE_DIR)/utils/uf2conv.py .
	chmod a+x uf2conv.py

$(CURDIR)/uf2families.json:
	cp $(PKG_SOURCE_DIR)/utils/uf2families.json .
