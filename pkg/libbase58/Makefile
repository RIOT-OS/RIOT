PKG_NAME=libbase58
PKG_URL=https://github.com/bitcoin/libbase58
PKG_VERSION=d7591398443987e84d19833d86634c6ffe8b0796
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
