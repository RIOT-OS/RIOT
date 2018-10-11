PKG_NAME=winc1500
PKG_URL=https://github.com/kbumsik/winc1500-projects
PKG_VERSION=396ea0cb3d10ef7099a15f352a3a6d792b16b48b
PKG_LICENSE=BSD-3-Clause-Atmel

VCS_DIR=$(PKG_BUILDDIR)/winc1500
MODULE_MAKEFILE := $(CURDIR)/Makefile.module

.PHONY: all

all: git-download
	# Copy Makefiles for each modules
	@cp Makefile.common $(VCS_DIR)/common/source/Makefile
	@cp Makefile.bsp $(VCS_DIR)/bsp/source/Makefile
	@cp Makefile.bus_wrapper $(VCS_DIR)/bus_wrapper/source/Makefile
	@cp Makefile.driver $(VCS_DIR)/driver/source/Makefile
	@cp Makefile.socket $(VCS_DIR)/socket/source/Makefile
	@cp Makefile.spi_flash $(VCS_DIR)/spi_flash/source/Makefile

	# Lastly, call Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.winc1500

include $(RIOTBASE)/pkg/pkg.mk
