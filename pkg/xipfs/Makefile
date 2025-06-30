PKG_NAME=xipfs
PKG_URL=https://github.com/2xs/xipfs.git
PKG_VERSION=962c5edc55abb6363ff2988dc3b2c3c293362e96
PKG_LICENSE=CeCILL-2.1

include $(RIOTBASE)/pkg/pkg.mk

export RIOT_INCLUDES=$(INCLUDES)
export RIOT_CFLAGS=$(CFLAGS)

all: $(BINDIR)/xipfs.a

$(BINDIR)/xipfs.a:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
	cp $(PKG_SOURCE_DIR)/$(BOARD)/xipfs.a $@

.PHONY: $(BINDIR)/xipfs.a
