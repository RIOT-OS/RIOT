PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=acb46aaed14ce607040771c2d9805140a5f482e4 # v7.3.0
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

LVGL_MODULES =      \
    lvgl_core       \
    lvgl_draw       \
    lvgl_font       \
    lvgl_hal        \
    lvgl_misc       \
    lvgl_themes     \
    lvgl_widgets    \
    #

.PHONY: lvgl_%

all: $(LVGL_MODULES)

lvgl_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/lv_$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@
