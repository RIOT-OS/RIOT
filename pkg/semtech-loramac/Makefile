PKG_NAME=semtech-loramac
PKG_URL=https://github.com/Lora-net/LoRaMac-node.git
PKG_VERSION=1cdd9ccec4c9f05b616e7112059be4a9e358c571
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# Use -isystem instead of -I to disable warnings for these headers
INCLUDES += -isystem$(PKGDIRBASE)/semtech-loramac/src/mac \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/boards/mcu \
            -isystem$(PKGDIRBASE)/semtech-loramac/src/system/crypto \
            -isystem$(PKGDIRBASE)/semtech-loramac/src

LORAMAC_MODULES = loramac_arch loramac_crypto loramac_mac loramac_region

DIR_loramac_arch = boards/mcu
DIR_loramac_crypto = system/crypto
DIR_loramac_mac = mac
DIR_loramac_region = mac/region

.PHONY: loramac_%

all: $(LORAMAC_MODULES)

loramac_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/$(DIR_$@) -f $(CURDIR)/Makefile.$@
