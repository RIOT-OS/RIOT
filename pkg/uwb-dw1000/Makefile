PKG_NAME=uwb-dw1000
PKG_URL=https://github.com/Decawave/uwb-dw1000/
PKG_VERSION=6eaa85e6d429450d19a6ddeb2de05303016c0dd2
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-address-of-packed-member
CFLAGS += -Wno-enum-conversion
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-missing-braces
CFLAGS += -Wno-missing-declarations
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-return-type
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -fms-extensions

ifneq (,$(filter llvm,$(TOOLCHAIN)))
  CFLAGS += -Wno-microsoft-anon-tag
endif

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/hw/drivers/uwb/uwb_dw1000/src -f $(RIOTPKG)/uwb-dw1000/uwb-dw1000.mk MODULE=uwb-dw1000
