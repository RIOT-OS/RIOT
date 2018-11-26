PKG_NAME=tinycrypt
PKG_URL=https://github.com/01org/tinycrypt
PKG_VERSION=6a22712bebbf2fc60d9fc6192dddefd5ad1933e3
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/lib/source/ \
			  -f $(RIOTPKG)/tinycrypt/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
