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

# SDL requires more stack
ifeq (native,$(BOARD))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=64*1024
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*1024
endif


include $(RIOTBASE)/Makefile.include
