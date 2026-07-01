PKG_NAME=yxml
PKG_URL=https://g.blicky.net/yxml.git
PKG_MIRROR_URL=https://github.com/RIOT-OS-pkgmirror/yxml.git
PKG_VERSION=f9438757fc49b9f86961ddb55ae430e36bb88ebb
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
