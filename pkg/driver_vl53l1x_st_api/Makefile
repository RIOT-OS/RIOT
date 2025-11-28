PKG_NAME=driver_vl53l1x_st_api
PKG_URL=https://github.com/gschorcht/riot_st_vl53l1x_uld_api.git
PKG_VERSION=ec885135dd145b4fad1c91ac3fe67b8e359a22b1
PKG_LICENSE=BSD

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core -f $(RIOTBASE)/Makefile.base MODULE=vl53l1x_st_api_core
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/platform -f $(RIOTBASE)/Makefile.base MODULE=vl53l1x_st_api_platform
