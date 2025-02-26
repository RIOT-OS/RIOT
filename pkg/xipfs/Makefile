PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=0f9d27a025bdcfe291fa3de7352a7d92d78b4dc8
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

export RIOT_INCLUDES=$(INCLUDES)
export RIOT_CFLAGS=$(CFLAGS)

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	@cp $(PKG_SOURCE_DIR)/xipfs.a $@
