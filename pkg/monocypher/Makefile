PKG_NAME=monocypher
PKG_URL=https://github.com/LoupVaillant/Monocypher
PKG_VERSION=baca5d31259c598540e4d1284bc8d8f793abf83a # v3.1.2
PKG_LICENSE=CC-0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -DBLAKE2_NO_UNROLLING

all: $(filter monocypher_optional,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)

monocypher_optional:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/optional -f $(RIOTBASE)/Makefile.base MODULE=$@
