PKG_NAME=fatfs
PKG_URL=https://github.com/MichelRottleuthner/FatFs_for_RIOT.git
PKG_VERSION=61fd6ae3815170bf7bf6121f33f1ef68c2b11599
PKG_LICENSE=BSD-1-Clause
MODULE_MAKEFILE := $(CURDIR)/Makefile.fatfs

.PHONY: all

all: git-download
	@cp $(MODULE_MAKEFILE) $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
