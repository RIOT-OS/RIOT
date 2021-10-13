PKG_NAME=libcoap
PKG_URL=https://github.com/obgm/libcoap
PKG_VERSION=ef41ce5d02d64cec0751882ae8fd95f6c32bc018
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

# GCC 7.x fails on (intentional) fallthrough, thus disable implicit-fallthrough.
CFLAGS += -Wno-implicit-fallthrough

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-format-nonliteral
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)
