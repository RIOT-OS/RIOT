PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=80283faa05fa03fa2b5372f658b5173dd7f6d4c0
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

export RIOT_INCLUDES=$(INCLUDES)
export RIOT_CFLAGS=$(CFLAGS)

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	@cp $(PKG_SOURCE_DIR)/xipfs.a $@
