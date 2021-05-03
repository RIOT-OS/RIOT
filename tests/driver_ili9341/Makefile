BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

USEMODULE += ili9341
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# Check if being configured via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_ILI9341
CFLAGS += -DCONFIG_ILI9341_LE_MODE
endif

# The AVR architecture stores the image in the RAM, this usually doesn't fit.
# This flag excludes the image from the test
ifneq (,$(filter arch_avr8,$(FEATURES_USED)))
  CFLAGS += -DNO_RIOT_IMAGE
endif
