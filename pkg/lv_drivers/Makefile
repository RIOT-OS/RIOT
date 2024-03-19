PKG_NAME=lv_drivers
PKG_URL=https://github.com/lvgl/lv_drivers.git
# 8.2.0
PKG_VERSION=49c4b178494625efefb07891d1c8b9c13914edff
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

LV_DRIVERS_MODULES := $(filter lv_drivers_%,$(USEMODULE))

.PHONY: lv_drivers_%

all: $(LV_DRIVERS_MODULES)

lv_drivers_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/$* -f $(CURDIR)/Makefile.lv_drivers_module MODULE=$@ SRC=$*.c
