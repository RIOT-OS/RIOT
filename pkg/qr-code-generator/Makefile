PKG_NAME=qr-code-generator
PKG_URL=https://github.com/nayuki/QR-Code-generator
PKG_VERSION=4d13c303dc878ffae8df6d0e0a7513fe860a092c  # v1.7.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-type-limits

# avoid compilation errors when NDEBUG is defined
CFLAGS += -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/c -f $(CURDIR)/$(PKG_NAME).mk
