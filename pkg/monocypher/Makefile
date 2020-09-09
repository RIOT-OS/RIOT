PKG_NAME=monocypher
PKG_URL=https://github.com/LoupVaillant/Monocypher
PKG_VERSION=ff334e288a667c5cd8500c04d1e2ebd601b9f215
PKG_LICENSE=CC-0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DBLAKE2_NO_UNROLLING

all: $(filter monocypher_optional,$(USEMODULE))
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

monocypher_optional:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/optional -f $(RIOTBASE)/Makefile.base MODULE=$@
