PKG_NAME=qr-code-generator
PKG_URL=https://github.com/nayuki/QR-Code-generator
# v1.8.0
PKG_VERSION=720f62bddb7226106071d4728c292cb1df519ceb
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-type-limits

# avoid compilation errors when NDEBUG is defined
CFLAGS += -Wno-unused-parameter

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/c -f $(CURDIR)/$(PKG_NAME).mk
