PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=5035c0c7809d288d5b5d141a22b0bf9bf932cf1b # v7.7.1
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
