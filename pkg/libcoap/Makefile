PKG_NAME=libcoap
PKG_URL=https://github.com/obgm/libcoap
PKG_VERSION=ef41ce5d02d64cec0751882ae8fd95f6c32bc018
PKG_LICENSE=BSD-2-Clause

# GCC 7.x fails on (intentional) fallthrough, thus disable implicit-fallthrough.
CFLAGS += -Wno-implicit-fallthrough

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-format-nonliteral
endif
