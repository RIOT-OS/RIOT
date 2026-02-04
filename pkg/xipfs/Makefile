PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=26b606a3413b4c0f28ccea53ec79b2cd0ad99d28
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DXIPFS_ENABLE_SAFE_EXEC_SUPPORT

export RIOT_INCLUDES=$(INCLUDES)
export RIOT_CFLAGS=$(CFLAGS)

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	cp $(PKG_SOURCE_DIR)/$(BOARD)/xipfs.a $@

.PHONY: $(BINDIR)/xipfs.a
