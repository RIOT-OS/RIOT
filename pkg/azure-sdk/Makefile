PKG_NAME = azure-sdk
PKG_URL = https://github.com/Azure/azure-sdk-for-c
PKG_VERSION = 1.5.0
PKG_LICENSE = MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sdk/src/azure/core/ -f $(CURDIR)/Makefile.$(PKG_NAME)-core
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/sdk/src/azure/iot/ -f $(CURDIR)/Makefile.$(PKG_NAME)
