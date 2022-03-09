BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += touch_dev
USEMODULE += ztimer
USEMODULE += ztimer_msec

include $(RIOTBASE)/Makefile.include
