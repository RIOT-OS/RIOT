PKG_NAME=cn-cbor
PKG_URL=https://github.com/jimsch/cn-cbor
PKG_VERSION=821797f9421c04dc783561dc465066857839e108 # 1.0.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# Enable code forcing aligned reads
CFLAGS += -DCBOR_ALIGN_READS
CFLAGS += -Wno-return-local-addr
CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
