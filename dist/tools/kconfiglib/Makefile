PKG_NAME=kconfiglib
PKG_URL=https://github.com/ulfalizer/Kconfiglib
PKG_VERSION=061e71f7d78cb057762d88de088055361863deff
PKG_LICENSE=ISC

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(Q)cp $(PKG_SOURCE_DIR)/kconfiglib.py $(PKG_SOURCE_DIR)/menuconfig.py \
	   $(PKG_SOURCE_DIR)/genconfig.py $(PKG_SOURCE_DIR)/examples/merge_config.py \
	   .

remove:
	$(Q)$(RM) -r $(PKG_SOURCE_DIR) kconfiglib.py menuconfig.py genconfig.py \
	       merge_config.py
