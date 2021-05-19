BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += auto_init_screen
USEMODULE += touch_dev
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
