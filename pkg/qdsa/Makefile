PKG_NAME=qdsa
PKG_URL=https://github.com/RIOT-OS/qdsa.git
PKG_VERSION=3fa25ffa3971000fe4a3e3b42340c40c8d79f2a2
PKG_LICENSE=PD

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(patsubst qdsa_impl_%,%,$(filter qdsa_impl_%,$(USEMODULE)))
