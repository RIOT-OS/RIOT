PKG_NAME=meshtastic
PKG_URL=https://github.com/Meshtastic-Solutions/libmeshtastic-leaf
PKG_VERSION=bd542d6e77aa7b9507dcc8d153f6dad0c35a8c17
PKG_LICENSE=GPL

# take only the /src folder inside the remote upstream into account
PKG_SPARSE_PATHS += src

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: meshtastic%

all: meshtastic meshtastic_generated meshtastic_third_party

meshtastic:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.meshtastic

meshtastic_generated:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/generated/meshtastic -f $(CURDIR)/Makefile.meshtastic_generated

meshtastic_third_party:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/third_party -f $(CURDIR)/Makefile.meshtastic_third_party
