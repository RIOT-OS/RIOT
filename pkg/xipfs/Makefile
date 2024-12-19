PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=0d04bd4d2e38ee17313b3ef49fcbb71eee1bb64c
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a: $(PKG_SOURCE_DIR)/toolchain.mk
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	@cp $(PKG_SOURCE_DIR)/xipfs.a $@

$(PKG_SOURCE_DIR)/toolchain.mk:
	$(PKG_SOURCE_DIR)/configure.sh --board=$(BOARD)
