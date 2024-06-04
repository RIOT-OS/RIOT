PKG_NAME=monocypher
PKG_URL=https://github.com/LoupVaillant/Monocypher
# v4.0.2
PKG_VERSION=0d85f98c9d9b0227e42cf795cb527dff372b40a4
PKG_LICENSE=CC-0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DBLAKE2_NO_UNROLLING

all: $(filter monocypher_optional,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

monocypher_optional:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/optional -f $(RIOTBASE)/Makefile.base MODULE=$@
