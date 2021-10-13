PKG_NAME=uwb-dw1000
PKG_URL=https://github.com/Decawave/uwb-dw1000/
PKG_VERSION=d44078a96349b7a40e9c2393ea83ca4c2d53ab92
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-enum-compare
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
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/hw/drivers/uwb/uwb_dw1000/src -f $(RIOTPKG)/uwb-dw1000/uwb-dw1000.mk MODULE=uwb-dw1000
