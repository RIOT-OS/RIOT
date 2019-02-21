PKG_NAME=semtech-loramac
PKG_URL=https://github.com/Lora-net/LoRaMac-node.git
PKG_VERSION=1cdd9ccec4c9f05b616e7112059be4a9e358c571
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	@cp Makefile.loramac $(PKG_BUILDDIR)/Makefile
	@cp Makefile.loramac_mac $(PKG_BUILDDIR)/src/mac/Makefile
	@cp Makefile.loramac_region $(PKG_BUILDDIR)/src/mac/region/Makefile
	@cp Makefile.loramac_crypto $(PKG_BUILDDIR)/src/system/crypto/Makefile
	@cp Makefile.loramac_arch $(PKG_BUILDDIR)/src/boards/mcu/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
