PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=da7c6a73129e549ce17d1425536b33c5315728e0
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a: $(PKG_SOURCE_DIR)/toolchain.mk
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	@cp $(PKG_SOURCE_DIR)/xipfs.a $@

$(PKG_SOURCE_DIR)/toolchain.mk:
	$(PKG_SOURCE_DIR)/configure.sh --board=$(BOARD)
