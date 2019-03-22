PKG_NAME=driver_vl53l1x_st_api
PKG_URL=https://github.com/pololu/vl53l1x-st-api-arduino.git
PKG_VERSION=5713cac245a6568273b9d5def7880c50ef7ddde0
PKG_LICENSE=BSD3

.PHONY: all

all: git-download
	rm -f $(PKG_BUILDDIR)/vl53l1x-st-api/vl53l1x-st-api.ino
	rm -f $(PKG_BUILDDIR)/vl53l1x-st-api/vl53l1_platform.c
	rm -f $(PKG_BUILDDIR)/vl53l1x-st-api/vl53l1_platform.cpp
	"$(MAKE)" -C $(PKG_BUILDDIR)/vl53l1x-st-api -f $(CURDIR)/Makefile.driver_vl53l1x_st_api

include $(RIOTBASE)/pkg/pkg.mk
