PKG_NAME=tinycrypt
PKG_URL=https://github.com/01org/tinycrypt
PKG_VERSION=5969b0e0f572a15ed95dc272e57104faeb5eb6b0
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/lib/source/ -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
