PKG_NAME=qdsa
PKG_URL=https://github.com/RIOT-OS/qdsa.git
PKG_VERSION=4cb3f1a140f25e18ed288fd484defe3d45bdf166
PKG_LICENSE=PD

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(patsubst qdsa_impl_%,%,$(filter qdsa_impl_%,$(USEMODULE)))
