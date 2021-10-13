PKG_NAME=UF2
PKG_URL=https://github.com/microsoft/uf2.git
PKG_VERSION=41394bd604e341ebcfdf0a8c4acce9ec8fd73d93
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/uf2conv.py

$(CURDIR)/uf2conv.py:
	cp $(PKG_SOURCE_DIR)/utils/uf2conv.py .
	chmod a+x uf2conv.py
