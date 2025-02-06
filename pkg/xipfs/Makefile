PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=374f4cba72018f8f3d4e62327f01d8657e6705a4
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a: $(PKG_SOURCE_DIR)/toolchain.mk
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	@cp $(PKG_SOURCE_DIR)/xipfs.a $@

$(PKG_SOURCE_DIR)/toolchain.mk:
	$(PKG_SOURCE_DIR)/configure.sh --board=$(BOARD)
