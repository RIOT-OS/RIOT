BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

# No interactive_sync
DISABLE_MODULE += test_utils_interactive_sync

USEPKG += lvgl
USEMODULE += lvgl_contrib

# Add touch capabilities
USEMODULE += lvgl_contrib_touch

include $(RIOTBASE)/Makefile.include
