PKG_NAME=lvgl
PKG_URL=https://github.com/lvgl/lvgl
# v9.6.0
PKG_VERSION=80ca777e37a2b176770726a02e07a6fb79ef0b39
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-empty-body
CFLAGS += -fno-jump-tables
# osal/lv_linux.c, built on native, passes its fscanf() format in a variable
CFLAGS += -Wno-format-nonliteral

# Source directories (relative to src/) always built by the lvgl_core module.
# Optional parts are disabled from lv_conf.h, so the files they contain build
# to empty objects.
LVGL_CORE_DIRS =            \
    .                       \
    core                    \
    display                 \
    draw                    \
    draw/convert            \
    draw/sw                 \
    draw/sw/blend           \
    font                    \
    font/fmt_txt            \
    fs                      \
    image                   \
    indev                   \
    layouts                 \
    logging                 \
    misc                    \
    misc/cache              \
    misc/cache/class        \
    misc/cache/instance     \
    osal                    \
    stdlib                  \
    stdlib/builtin          \
    stdlib/clib             \
    themes                  \
    tick                    \
    #

LVGL_LAYOUT_MODULES =       \
    lvgl_layout_flex        \
    lvgl_layout_grid        \
    #

LVGL_THEME_MODULES =        \
    lvgl_theme_default      \
    lvgl_theme_mono         \
    lvgl_theme_simple       \
    #

LVGL_WIDGET_MODULES =           \
    lvgl_widget_animimage       \
    lvgl_widget_arc             \
    lvgl_widget_bar             \
    lvgl_widget_button          \
    lvgl_widget_buttonmatrix    \
    lvgl_widget_calendar        \
    lvgl_widget_canvas          \
    lvgl_widget_chart           \
    lvgl_widget_checkbox        \
    lvgl_widget_dropdown        \
    lvgl_widget_image           \
    lvgl_widget_imagebutton     \
    lvgl_widget_keyboard        \
    lvgl_widget_label           \
    lvgl_widget_led             \
    lvgl_widget_line            \
    lvgl_widget_list            \
    lvgl_widget_menu            \
    lvgl_widget_msgbox          \
    lvgl_widget_roller          \
    lvgl_widget_scale           \
    lvgl_widget_slider          \
    lvgl_widget_span            \
    lvgl_widget_spinbox         \
    lvgl_widget_spinner         \
    lvgl_widget_switch          \
    lvgl_widget_table           \
    lvgl_widget_tabview         \
    lvgl_widget_textarea        \
    lvgl_widget_tileview        \
    lvgl_widget_win             \
    #

LVGL_MODULES = lvgl_core lvgl_sdl $(LVGL_LAYOUT_MODULES) $(LVGL_THEME_MODULES) $(LVGL_WIDGET_MODULES)
LVGL_MODULES_USED = $(filter $(LVGL_MODULES),$(USEMODULE))

.PHONY: lvgl_%

all: $(LVGL_MODULES_USED)

lvgl_core:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.lvgl_module MODULE=$@ SRC_DIRS="$(LVGL_CORE_DIRS)"

lvgl_sdl:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/drivers/sdl -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_layout_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/layouts/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_theme_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/themes/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_widget_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/widgets/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@
