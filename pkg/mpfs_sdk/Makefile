PKG_NAME=mpfs_sdk
PKG_URL=https://github.com/polarfire-soc/platform
PKG_VERSION=13c8cc74e445008bf8615097d44ea28af4ff8d97
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

ifneq ($(CPU),mpfs)
  $(error This package can only be used with MPFS CPUs)
endif

MPFS_MODULES = mpfs_sdk mpfs_hal


.PHONY = mpfs_%

all: $(MPFS_MODULES)

# First copy all needed files and gather them in a single place
mpfs_sdk:
	@mkdir -p $(PKG_BUILD_DIR)
	@cp $(PKG_SOURCE_DIR)/drivers/mss/mss_gpio/mss_gpio.c $(PKG_BUILD_DIR)/
	@cp $(PKG_SOURCE_DIR)/drivers/mss/mss_mmuart/mss_uart.c $(PKG_BUILD_DIR)/

	$(QQ)"$(MAKE)" -C $(PKG_BUILD_DIR)/ -f $(CURDIR)/Makefile.$@

mpfs_hal:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/mpfs_hal/common -f $(CURDIR)/Makefile.$@