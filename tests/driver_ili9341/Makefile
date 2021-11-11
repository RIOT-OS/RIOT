BOARD ?= stm32f429i-disc1
include ../Makefile.tests_common

USEMODULE += ili9341
USEMODULE += ztimer
USEMODULE += ztimer_msec

# As there is an 'Kconfig' we want to explicitly disable Kconfig by setting
# the variable to empty
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include

# Check if being configured via Kconfig
ifndef CONFIG_KCONFIG_USEMODULE_ILI9341
  CFLAGS += -DCONFIG_ILI9341_LE_MODE

  # The AVR architecture stores the image in the RAM, this usually doesn't fit.
  # This flag excludes the image from the test
  ifneq (,$(filter arch_avr8,$(FEATURES_USED)))
    CFLAGS += -DCONFIG_NO_RIOT_IMAGE
  endif
endif
