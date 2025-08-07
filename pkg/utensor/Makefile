PKG_NAME=utensor
PKG_URL=https://github.com/uTensor/uTensor
PKG_VERSION=b22aa5cbc1ef1b3dd42a899b393a9b999afd1305
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/uTensor/core -f $(CURDIR)/Makefile.$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/uTensor/util -f $(CURDIR)/Makefile.$(PKG_NAME).util
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/uTensor/ops -f $(CURDIR)/Makefile.$(PKG_NAME).ops
