BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

USEPKG += lvgl
USEMODULE += lvgl_contrib
USEMODULE += lvgl_extra_widget_chart
USEMODULE += lvgl_extra_widget_win
USEMODULE += lvgl_extra_layout_flex
USEMODULE += lvgl_extra_theme_default
USEMODULE += lvgl_extra_theme_default_dark

CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*1024

include $(RIOTBASE)/Makefile.include
