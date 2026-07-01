PKG_NAME=elk
PKG_URL=https://github.com/cesanta/elk
# 2.1.0
PKG_VERSION=1eab37bd9041d93a0bdfcd4335b2dcdba1076c67
PKG_LICENSE=GPL-2.0-only

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align
CFLAGS += -Wno-format-nonliteral
CFLAGS += -Wno-pedantic
CFLAGS += -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/$(PKG_NAME).mk
