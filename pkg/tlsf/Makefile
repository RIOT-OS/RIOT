PKG_NAME=tlsf
PKG_URL=https://github.com/mattconte/tlsf
PKG_VERSION=a1f743ffac0305408b39e791e0ffb45f6d9bc777
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
