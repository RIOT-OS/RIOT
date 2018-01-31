PKG_NAME=fatfs
PKG_URL=https://github.com/RIOT-OS/FatFS
PKG_VERSION=34f371c7735fc6fc8e714a74a7a73a61d3ed5633
PKG_LICENSE=BSD-1-Clause
MODULE_MAKEFILE := $(CURDIR)/Makefile.fatfs

.PHONY: all

all: git-download
	@cp $(MODULE_MAKEFILE) $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
