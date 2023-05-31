PKG_NAME=kconfiglib
PKG_URL=https://github.com/RIOT-OS/Kconfiglib
PKG_VERSION=110688d78edba9fa7eb09cbe001c62fbbee86abf
PKG_LICENSE=ISC

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(Q)cp $(PKG_SOURCE_DIR)/kconfiglib.py $(PKG_SOURCE_DIR)/menuconfig.py \
	   .

remove:
	$(Q)$(RM) -r $(PKG_SOURCE_DIR) kconfiglib.py menuconfig.py
