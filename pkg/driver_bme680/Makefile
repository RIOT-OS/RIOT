PKG_NAME=driver_bme680
PKG_URL=https://github.com/BoschSensortec/BME680_driver
PKG_VERSION=63bb5336db4659519860832be2738c685133aa33
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter bme680_fp,$(USEMODULE)))
  CFLAGS += -DBME680_FLOAT_POINT_COMPENSATION
endif

.PHONY: all

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
