PKG_NAME=qr-code-generator
PKG_URL=https://github.com/nayuki/QR-Code-generator
PKG_VERSION=71c75cfeb0f06788ebc43a39b704c39fcf5eba7c  # v1.6.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-type-limits

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/c -f $(CURDIR)/$(PKG_NAME).mk
