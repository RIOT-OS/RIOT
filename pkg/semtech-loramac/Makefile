PKG_NAME=semtech-loramac
PKG_URL=https://github.com/Lora-net/LoRaMac-node.git
PKG_VERSION=f42be67be402a40b3586724800771bfe13fb18e6
PKG_LICENSE=BSD-3-Clause

.PHONY: all

all: git-download
	@cp Makefile.loramac $(PKG_BUILDDIR)/Makefile
	@cp Makefile.loramac_mac $(PKG_BUILDDIR)/src/mac/Makefile
	@cp Makefile.loramac_region $(PKG_BUILDDIR)/src/mac/region/Makefile
	@cp Makefile.loramac_crypto $(PKG_BUILDDIR)/src/system/crypto/Makefile
	@cp Makefile.loramac_arch $(PKG_BUILDDIR)/src/boards/mcu/stm32/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
