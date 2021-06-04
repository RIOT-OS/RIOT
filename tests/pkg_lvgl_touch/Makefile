BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

USEPKG += lvgl
USEMODULE += lvgl_contrib

# Add touch capabilities
USEMODULE += lvgl_contrib_touch
USEMODULE += lvgl_widget_btn
USEMODULE += lvgl_widget_label
USEMODULE += lvgl_extra_theme_default
# uncomment the following to enable growing button (needs more RAM)
# USEMODULE += lvgl_extra_theme_default_grow

CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*1024

include $(RIOTBASE)/Makefile.include
