BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += disp_dev

include $(RIOTBASE)/Makefile.include

# Check if being configured via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_LCD
  CFLAGS += -DCONFIG_LCD_LE_MODE
endif
