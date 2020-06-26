PKG_NAME=kconfiglib
PKG_URL=https://github.com/ulfalizer/Kconfiglib
PKG_VERSION=82f59179b1b35fcd8b6d188453b283599ea70518
PKG_LICENSE=ISC

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(Q)cp $(PKG_SOURCE_DIR)/kconfiglib.py $(PKG_SOURCE_DIR)/menuconfig.py \
	   $(PKG_SOURCE_DIR)/genconfig.py $(PKG_SOURCE_DIR)/examples/merge_config.py \
	   .

remove:
	$(Q)$(RM) -r $(PKG_SOURCE_DIR) kconfiglib.py menuconfig.py genconfig.py \
	       merge_config.py
