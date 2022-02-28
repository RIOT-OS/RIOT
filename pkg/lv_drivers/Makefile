PKG_NAME=lv_drivers
PKG_URL=https://github.com/lvgl/lv_drivers.git
PKG_VERSION=49c4b178494625efefb07891d1c8b9c13914edff # 8.2.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

IGNORE_MODULES := \
                  #

LV_DRIVERS_MODULES := $(filter-out $(IGNORE_MODULES),$(filter lv_drivers_%,$(USEMODULE)))

.PHONY: lvgl_%

all: $(LV_DRIVERS_MODULES)

lv_drivers_sdl:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/sdl -f $(CURDIR)/Makefile.lv_drivers_module MODULE=$@ SRC=sdl.c
