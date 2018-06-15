PKG_NAME=tinycrypt
PKG_URL=https://github.com/01org/tinycrypt
PKG_VERSION=3ea1a609e7aff9f2d8d13803e1076b7a8e551804
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/lib/source/ \
			  -f $(RIOTPKG)/tinycrypt/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
