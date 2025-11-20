BOARD ?= stm32f429i-disc1
include ../Makefile.pkg_common

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

USEMODULE += random

include $(RIOTBASE)/Makefile.include

# SDL requires more stack
ifneq (,$(filter native native32 native64,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=64*1024
else ifneq (,$(filter esp%,$(CPU_FAM)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=4*1024
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*1024
endif
