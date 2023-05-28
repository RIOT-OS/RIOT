PKG_NAME=lvgl
PKG_URL=https://github.com/lvgl/lvgl
# v8.3.7
PKG_VERSION=2b56e04205481daa6575bd5f7ab5df59d11676eb
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-empty-body

LVGL_DEFAULT_MODULES =  \
    lvgl_core           \
    lvgl_draw           \
    lvgl_draw_sw        \
    lvgl_extra          \
    lvgl_font           \
    lvgl_hal            \
    lvgl_misc           \
    lvgl_widgets        \
    #

LVGL_EXTRA_LAYOUTS_MODULES =  \
    lvgl_extra_layout_flex   \
    lvgl_extra_layout_grid \
    #

LVGL_EXTRA_THEMES_MODULES =  \
    lvgl_extra_theme_basic   \
    lvgl_extra_theme_default \
    lvgl_extra_theme_mono \
    #

LVGL_EXTRA_WIDGETS_MODULES =  \
    lvgl_extra_widget_animimg \
    lvgl_extra_widget_calendar \
    lvgl_extra_widget_chart   \
    lvgl_extra_widget_colorwheel \
    lvgl_extra_widget_imgbtn \
    lvgl_extra_widget_keyboard   \
    lvgl_extra_widget_led \
    lvgl_extra_widget_list   \
    lvgl_extra_widget_menu \
    lvgl_extra_widget_meter \
    lvgl_extra_widget_msgbox \
    lvgl_extra_widget_span   \
    lvgl_extra_widget_spinbox   \
    lvgl_extra_widget_spinner   \
    lvgl_extra_widget_tabview \
    lvgl_extra_widget_tileview   \
    lvgl_extra_widget_win \
    #

LVGL_MODULES = $(LVGL_DEFAULT_MODULES) $(LVGL_EXTRA_LAYOUTS_MODULES) $(LVGL_EXTRA_THEMES_MODULES) $(LVGL_EXTRA_WIDGETS_MODULES)
LVGL_MODULES_USED = $(filter $(LVGL_MODULES),$(USEMODULE))

.PHONY: lvgl_%

all: $(LVGL_MODULES_USED)

lvgl_extra_layout_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/layouts/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_extra_theme_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/themes/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_extra_widget_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/widgets/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_extra:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_draw_sw:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/draw/sw -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@
