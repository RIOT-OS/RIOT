PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=ec9de515b36641be565d7bace5863ab631ce3b69 # v7.11.0
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
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/lv_$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@
