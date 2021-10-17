PKG_NAME=corejson
PKG_URL=https://github.com/FreeRTOS/coreJSON
PKG_VERSION=caf540ccdb98e8f96a6f557075cb607288384938  # v3.0.2
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/source -f $(CURDIR)/$(PKG_NAME).mk
