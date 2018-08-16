PKG_NAME=winc1500
PKG_URL=https://github.com/kbumsik/winc1500-projects
PKG_VERSION=396ea0cb3d10ef7099a15f352a3a6d792b16b48b
PKG_LICENSE=BSD-3-Clause-Atmel

PKG_SRCDIR=$(PKG_BUILDDIR)/winc1500
MODULE_MAKEFILE := $(CURDIR)/Makefile.module

.PHONY: all

all: git-download
	# Copy Makefiles for each modules
	echo 'MODULE = winc1500_common' > $(PKG_SRCDIR)/common/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/common/source/Makefile

	echo 'MODULE = winc1500_bsp' > $(PKG_SRCDIR)/bsp/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/bsp/source/Makefile

	echo 'MODULE = winc1500_bus_wrapper' > $(PKG_SRCDIR)/bus_wrapper/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/bus_wrapper/source/Makefile

	echo 'MODULE = winc1500_driver' > $(PKG_SRCDIR)/driver/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/driver/source/Makefile

	echo 'MODULE = winc1500_socket' > $(PKG_SRCDIR)/socket/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/socket/source/Makefile

	echo 'MODULE = winc1500_spi_flash' > $(PKG_SRCDIR)/spi_flash/source/Makefile
	cat $(MODULE_MAKEFILE) >> $(PKG_SRCDIR)/spi_flash/source/Makefile

	# Lastly, call Makefile
	"$(MAKE)" -C $(PKG_SRCDIR) -f $(CURDIR)/Makefile.winc1500

include $(RIOTBASE)/pkg/pkg.mk
