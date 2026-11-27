BOARD ?= stm32f429i-disc1
include ../Makefile.pkg_common

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

USEPKG += lvgl
USEMODULE += lvgl_contrib

# Add touch capabilities
USEMODULE += lvgl_contrib_touch
USEMODULE += lvgl_widget_button
USEMODULE += lvgl_widget_label
USEMODULE += lvgl_theme_default
# uncomment the following to enable growing button (needs more RAM)
# USEMODULE += lvgl_theme_default_grow

USEMODULE += random

include $(RIOTBASE)/Makefile.include

# SDL and the libraries of the host display require more stack, 64 KiB
# overflow on native64; LVGL v9 uses about 4.1 KiB of the main stack on a
# Cortex-M7
ifneq (,$(filter native native32 native64,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=128*1024
else ifneq (,$(filter esp%,$(CPU_FAM)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=8*1024
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=6*1024
endif
