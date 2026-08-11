
MODULE_DEFS_NEEDED += \
  periph_gpio \
  periph_gpio_irq \
  periph_rtc \
  pm_layered \
  #
BOARD ?= slstk3401a
include ../Makefile.periph_common

FEATURES_OPTIONAL += periph_rtc
FEATURES_OPTIONAL += periph_gpio_irq

USEMODULE += shell_cmds_default

# avoid running Kconfig by default
SHOULD_RUN_KCONFIG ?=

include $(RIOTBASE)/Makefile.include
