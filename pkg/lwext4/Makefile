PKG_NAME=lwext4
PKG_URL=https://github.com/gkostka/lwext4.git
PKG_VERSION=58bcf89a121b72d4fb66334f1693d3b30e4cb9c5
PKG_LICENSE=GPL-2.0-only

CFLAGS += -Wno-cast-align
CFLAGS += -Wno-error=format
CFLAGS += -Wno-unused-parameter -Wno-unused-variable

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
