PKG_NAME=lv_drivers
PKG_URL=https://github.com/lvgl/lv_drivers.git
PKG_VERSION=8c96359f4199297809ab205870eb603b03d98b4e # v7.11.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

IGNORE_MODULES := lv_drivers_monitor \
                  lv_drivers_mouse \
                  #

LV_DRIVERS_MODULES := $(filter-out $(IGNORE_MODULES),$(filter lv_drivers_%,$(USEMODULE)))

.PHONY: lvgl_%

all: $(LV_DRIVERS_MODULES)

lv_drivers_display_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/display -f $(CURDIR)/Makefile.lv_drivers_module MODULE=$@ SRC=$*.c

lv_drivers_indev_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/indev -f $(CURDIR)/Makefile.lv_drivers_module MODULE=$@ SRC=$*.c
