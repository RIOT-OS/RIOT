PKG_NAME=baselibc
PKG_URL=https://github.com/PetteriAimonen/Baselibc.git
PKG_VERSION=7e8e4a7353b1fd381e466349ff15bf821f688194
PKG_LICENSE=BSD

.PHONY: all

all: Makefile.baselibc
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(abspath $<)

include $(RIOTBASE)/pkg/pkg.mk
