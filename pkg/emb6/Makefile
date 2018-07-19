PKG_NAME=emb6
PKG_URL=https://github.com/hso-esk/emb6.git
PKG_VERSION=14e4a3cfff01644e078870e14e16a1fe60dcc895
PKG_LICENSE=BSD-3-Clause

# GCC 7.x fails on (intentional) fallthrough, thus disable implicit-fallthrough.
CFLAGS += -Wno-implicit-fallthrough
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-strict-prototypes

# Collect all chosen sub-modules but not the pseudo-modules and the ones
# implemented in ./contrib/
EMB6_SUBMODULES:=$(filter-out emb6_contrib \
                              emb6_netdev \
                              emb6_router \
                              emb6_sock_%,$(filter emb6_%,$(USEMODULE)))

.PHONY: all

all: git-download $(EMB6_SUBMODULES)
	"$(MAKE)" -C $(PKG_BUILDDIR)

# Rule for all submodules
emb6_%: git-download
	"$(MAKE)" -C $(dir $(shell grep -lR "MODULE.*=.*\<$@\>" $(PKG_BUILDDIR)))

include $(RIOTBASE)/pkg/pkg.mk

ifeq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-tautological-compare
  CFLAGS += -Wno-parentheses-equality
endif
